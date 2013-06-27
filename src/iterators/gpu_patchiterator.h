#ifndef GPU_PATCHITERATOR_H
#define GPU_PATCHITERATOR_H

#include "tpatchiterator.h"
#include "gpu_patch.h"
#include "cudatools.h"


template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
class GPU_PatchIterator : public TPatchIterator<T_CPU, DIM, OP>
{

protected: // attributes

  bool          m_GpuPointersSet;
  vector<T_GPU> m_GpuPatches;
  size_t        m_MaxNumThreads;


public:

  GPU_PatchIterator(OP op);

  CUDA_HO void updateHost();
  CUDA_HO void updateDevice();

  virtual void addPatch(Patch *patch);
  virtual void copyField(size_t i_src, size_t i_dst);
  virtual void copyDonorData(size_t i_field);

};


template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::GPU_PatchIterator(OP op)
  : TPatchIterator<T_CPU, DIM, OP>(op)
{
  m_GpuPointersSet = false;
  int count;
  if (cudaGetDeviceCount(&count) != cudaSuccess) {
    cerr << "error detecting CUDA devices" << endl;
    exit(EXIT_FAILURE);
  }
  if (count == 0) {
    cerr << "no CUDA devices found" << endl;
    exit(EXIT_FAILURE);
  }
  cudaDeviceProp prop;
  if (cudaGetDeviceProperties(&prop, 0) != cudaSuccess) {
    cerr << "error fetching device properties" << endl;
    exit(EXIT_FAILURE);
  }
  m_MaxNumThreads = min(512,min(prop.maxThreadsPerBlock, prop.maxThreadsDim[0]));
}

template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
void GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::addPatch(Patch *patch)
{
  T_CPU* cpu_patch = dynamic_cast<T_CPU*>(patch);
  if (cpu_patch == NULL) {
    BUG;
  }
  TPatchIterator<T_CPU, DIM, OP>::addPatch(cpu_patch);
  T_GPU gpu_patch(cpu_patch);
  m_GpuPatches.push_back(gpu_patch);
}

template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
void GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::copyField(size_t i_src, size_t i_dst)
{
  for (size_t i = 0; i < this->m_Patches.size(); ++i) {
    cudaMemcpy(m_GpuPatches[i].getField(i_dst), m_GpuPatches[i].getField(i_src), m_GpuPatches[i].fieldSize()*sizeof(real) ,cudaMemcpyDeviceToDevice);
  }
}

template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
void GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::updateHost()
{
  for (size_t i = 0; i < this->m_Patches.size(); ++i) {
    m_GpuPatches[i].copyFromDevice(this->m_Patches[i]);
  }
}

template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
void GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::updateDevice()
{
  cout << "void GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::updateDevice()" << endl;
  if (!m_GpuPointersSet) {
    for (size_t i = 0; i < m_GpuPatches.size(); ++i) {
      cout << "translating " << i << endl;
      m_GpuPatches[i].updateDonorPointers(PatchIterator::getPatch(i));
    }
    m_GpuPointersSet = true;
  }
  for (size_t i = 0; i < this->m_Patches.size(); ++i) {
    m_GpuPatches[i].copyToDevice(this->m_Patches[i]);
  }
}

template <typename T_GPU>
__global__ void GPU_PatchIterator_kernelResetReceiverData(T_GPU patch, size_t i_field)
{
  size_t i = blockDim.x*blockIdx.x + threadIdx.x;
  if (i < patch.getNumReceivingCellsUnique()) {
    size_t i_rec = patch.getReceivingCellIndicesUnique()[i];
    for (size_t i_var = 0; i_var < patch.numVariables(); ++i_var) {
      patch.getVariable(i_field, i_var)[i_rec] = 0;
    }
  }
}

template <typename T_GPU>
__global__ void GPU_PatchIterator_kernelCopyDonorData(T_GPU patch, size_t i_field, size_t i_donor)
{
  size_t i = blockDim.x*blockIdx.x + threadIdx.x;
  donor_t donor = patch.getDonors()[i_donor];
  if (i < donor.num_receiver_cells) {
    //.... receiving cells index
    //size_t i_rec = m_ReceivingCellIndicesConcat[donor.receiver_index_field_start + ll_rec];
    size_t i_rec = patch.getReceivingCellIndicesConcat()[donor.receiver_index_field_start + i];

    //.... start address in m_DonorCells/m_DonorWeights pattern
    //size_t l_doner_cells_start = donor.donor_wi_field_start + ll_rec * donor.stride;
    size_t i_donor_cells_start = donor.donor_wi_field_start + i*donor.stride;

    //.... loop for contributing cells
    for (size_t i_contrib = 0; i_contrib < donor.stride; ++i_contrib) {

      size_t i_wi              = i_donor_cells_start + i_contrib;      // index of donor cell in concatenated lists
      size_t donor_cell_index  = patch.getDonorIndexConcat()[i_wi];
      real   donor_cell_weight = patch.getDonorWeightConcat()[i_wi];

      //...... loop for variables
      for (size_t i_var = 0; i_var < patch.numVariables(); ++i_var) {
        //*(this_vars[i_var]+i_rec) += donor_vars[i_var][donor_cell_index] * donor_cell_weight;  // contribute to receiving cell
        real* dvar = donor.data + i_var*donor.variable_size;
        patch.getVariable(i_field, i_var)[i_rec] += dvar[donor_cell_index]*donor_cell_weight;
      }

      // transform vectorial variables
      for (size_t i_vec = 0; i_vec < patch.getNumVectorVars(); ++i_vec) {
        size_t i_var = patch.getVectorVarIndices()[i_vec];
        real u =   donor.axx*patch.getVariable(i_field, i_var + 0)[i_rec]
                 + donor.axy*patch.getVariable(i_field, i_var + 1)[i_rec]
                 + donor.axy*patch.getVariable(i_field, i_var + 2)[i_rec];
        real v =   donor.ayx*patch.getVariable(i_field, i_var + 0)[i_rec]
                 + donor.ayy*patch.getVariable(i_field, i_var + 1)[i_rec]
                 + donor.ayy*patch.getVariable(i_field, i_var + 2)[i_rec];
        real w =   donor.azx*patch.getVariable(i_field, i_var + 0)[i_rec]
                 + donor.azy*patch.getVariable(i_field, i_var + 1)[i_rec]
                 + donor.azy*patch.getVariable(i_field, i_var + 2)[i_rec];
        patch.getVariable(i_field, i_var + 0)[i_rec] = u;
        patch.getVariable(i_field, i_var + 1)[i_rec] = v;
        patch.getVariable(i_field, i_var + 2)[i_rec] = w;
      }
    }
  }
}

template <typename T_CPU, typename T_GPU, unsigned int DIM, typename OP>
void GPU_PatchIterator<T_CPU, T_GPU, DIM, OP>::copyDonorData(size_t i_field)
{
  // reset receiver cells
  for (size_t i_patch = 0; i_patch < this->m_Patches.size(); ++i_patch) {
    int N = PatchIterator::getPatch(i_patch)->getNumReceivingCellsUnique();
    int blocks  = N/m_MaxNumThreads + 1;
    GPU_PatchIterator_kernelResetReceiverData<<<blocks, m_MaxNumThreads>>>(m_GpuPatches[i_patch], i_field);
  }

  cudaThreadSynchronize();

  // compute interpolated data
  for (size_t i_patch = 0; i_patch < this->m_Patches.size(); ++i_patch) {
    for (size_t i_donor = 0; i_donor < m_GpuPatches[i_patch].getNumDonorPatches(); ++i_donor) {
      int N = PatchIterator::getPatch(i_patch)->getDonors()[i_donor].num_receiver_cells;
      int blocks  = N/m_MaxNumThreads + 1;
      GPU_PatchIterator_kernelCopyDonorData<<<blocks, m_MaxNumThreads>>>(m_GpuPatches[i_patch], i_field, i_donor);
      cudaThreadSynchronize();
    }
  }
}




#endif // GPU_PATCHITERATOR_H
