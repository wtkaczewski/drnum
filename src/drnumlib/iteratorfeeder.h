// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +                                                                      +
// + This file is part of DrNUM.                                          +
// +                                                                      +
// + Copyright 2013 numrax GmbH, enGits GmbH                              +
// +                                                                      +
// + DrNUM is free software: you can redistribute it and/or modify        +
// + it under the terms of the GNU General Public License as published by +
// + the Free Software Foundation, either version 3 of the License, or    +
// + (at your option) any later version.                                  +
// +                                                                      +
// + enGrid is distributed in the hope that it will be useful,            +
// + but WITHOUT ANY WARRANTY; without even the implied warranty of       +
// + MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        +
// + GNU General Public License for more details.                         +
// +                                                                      +
// + You should have received a copy of the GNU General Public License    +
// + along with enGrid. If not, see <http://www.gnu.org/licenses/>.       +
// +                                                                      +
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifndef ITERATORFEEDER_H
#define ITERATORFEEDER_H

//#include <cstddef>
//#include <string.h>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <vector>

#include "blockcfd.h"
#include "iterators/patchiterator.h"
#include "patchgrid.h"

#include <string>
using namespace std;

//class IteratorFeeder;

class IteratorFeeder
{

  vector<PatchIterator*> m_Iterators;

protected: // methods

public: // methods

  /**
   * Add an Iterator
   * @param
   */
  void addIterator(PatchIterator* iterator);

  void feed(PatchGrid& patch_grid);

};

#endif // ITERATORFEEDER_H