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
// + DrNUM is distributed in the hope that it will be useful,             +
// + but WITHOUT ANY WARRANTY; without even the implied warranty of       +
// + MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        +
// + GNU General Public License for more details.                         +
// +                                                                      +
// + You should have received a copy of the GNU General Public License    +
// + along with DrNUM. If not, see <http://www.gnu.org/licenses/>.        +
// +                                                                      +
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef mathvector_h
#define mathvector_h

template <class V> struct MathVector;

#include <iostream>
#include <vector>
#include <cmath>

// static array class for small vectors
// ====================================

template <class T, unsigned int DIM>
class StaticVector
{
public:

  typedef T value_type;

protected:

  T VALUE[DIM];

public:

  T& operator[](const unsigned int &i) { return VALUE[i]; }
  T operator[](const unsigned int &i) const { return VALUE[i]; }
  unsigned int size() const { return DIM; }
  T* data() { return VALUE; }
};


#include "drnum.h"

using namespace std;

#include "math/mathvector_structs.h"


// wrapper class for arrays to provide mathematical functionality
// =======================================================================================

template <class V>
struct MathVector : public V
{

  // types
  // -----
  typedef typename V::value_type scalar_t;
  typedef typename V::value_type value_type;


  // constructors
  // ------------

  MathVector() : V() {};
  MathVector(const MathVector<V>& vec);
  MathVector(const value_type *v);
  MathVector(const value_type v1, const value_type v2, const value_type v3, const value_type v4);
  MathVector(const value_type v1, const value_type v2, const value_type v3);
  MathVector(const value_type v1, const value_type v2);
  
  template <class L, class O, class R>
  MathVector(const ParseNode<L,O,R> &expr);


  // operators
  // ---------
  void operator=  (const MathVector<V> &vec);
  //void operator=  (MathVector<V> &vec);
  void operator=  (const std::vector<typename V::value_type> &vec);
  void operator-= (const MathVector<V> &vec); 
  void operator+= (const MathVector<V> &vec);
  void operator*= (const scalar_t s);

  // assignment to an expression
  template <class L, class O, class R> void operator= (const ParseNode<L,O,R> &expr);


  // other things
  // ------------
  MathVector<V> cross(const MathVector<V> &vec) const;
  void minimisePerCoord(const MathVector<V>& vec);
  void maximisePerCoord(const MathVector<V>& vec);
  scalar_t abs() const;
  scalar_t abs2() const;
  MathVector<V> normalise();
  scalar_t* c_array() const;
  unsigned int dim() { return this->size(); }


  // STL
  // ---
  class iterator;
  class const_iterator;

  class iterator
  {
    friend class const_iterator;
    MathVector<V> *vec;
    unsigned int i;

  public:
    typedef random_access_iterator_tag iterator_category;
    typedef typename V::value_type value_type;
    typedef unsigned int difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    iterator(MathVector<V> *a_vec = NULL, unsigned int an_i = 0) : vec(a_vec), i(an_i) {}
    bool operator==(const iterator &iter) const { return (iter.vec == vec) && (iter.i == i); }
    bool operator==(const const_iterator &iter) const;
    bool operator!=(const const_iterator &iter) const { return !operator==(iter); }
    iterator operator++() { ++i; return iterator(vec,i); }
    iterator operator++(int) { unsigned int j = i; ++i; return iterator(vec,j); }
    iterator operator--() { --i; return iterator(vec,i); }
    iterator operator--(int) { unsigned int j = i; --i; return iterator(vec,j); }
    void operator+=(unsigned int n) { i += n; }
    void operator-=(unsigned int n) { i -= n; }
    scalar_t& operator*() const { return (*vec)[i]; }
    scalar_t& operator[](unsigned int n) const { return (*vec)[i+n]; }
    bool operator<(const iterator &iter) const { return i < iter.i; }
    unsigned int operator-(const iterator &iter) const { return i - iter.i; }
  };

  class const_iterator
  {
    MathVector<V> *vec;
    unsigned int i;

  public:
    typedef random_access_iterator_tag iterator_category;
    typedef typename V::value_type value_type;
    typedef unsigned int difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    const_iterator(MathVector<V> *a_vec = NULL, unsigned int an_i = 0) : vec(a_vec), i(an_i) {}
    const_iterator(const iterator &iter) : vec(iter.vec), i(iter.i) {}
    void operator=(const iterator &iter) { vec = iter.vec; i = iter.i; }
    bool operator==(const const_iterator &iter) const { return (iter.vec == vec) && (iter.i == i); }
    bool operator==(const iterator &iter) const;
    bool operator!=(const iterator &iter) const { return !operator==(iter); }
    const_iterator operator++() { ++i; return iterator(vec,i); }
    const_iterator operator++(int) { unsigned int j = i; ++i; return iterator(vec,j); }
    const_iterator operator--() { --i; return iterator(vec,i); }
    const_iterator operator--(int) { unsigned int j = i; --i; return iterator(vec,j); }
    void operator+=(unsigned int n) { i += n; }
    void operator-=(unsigned int n) { i -= n; }
    scalar_t operator*() const { return (*vec)[i]; }
    scalar_t operator[](unsigned int n) const { return (*vec)[i+n]; }
    bool operator<(const iterator &iter) const { return i < iter.i; }
    unsigned int operator-(const iterator &iter) const { return i - iter.i; }
  };

  iterator       begin()       { return iterator(this); }
  iterator       end()         { return iterator(this,this->size()); }
  const_iterator begin() const { return iterator(this); }
  const_iterator end()   const { return iterator(this,this->size()); }
};




typedef MathVector<StaticVector<real,2> > vec2_t;
typedef MathVector<StaticVector<real,3> > vec3_t;
typedef MathVector<StaticVector<real,4> > vec4_t;
typedef MathVector<StaticVector<real,5> > vec5_t;
typedef MathVector<StaticVector<real,6> > vec6_t;

typedef MathVector<StaticVector<float,2> > fvec2_t;
typedef MathVector<StaticVector<float,3> > fvec3_t;
typedef MathVector<StaticVector<float,4> > fvec4_t;
typedef MathVector<StaticVector<float,5> > fvec5_t;
typedef MathVector<StaticVector<float,6> > fvec6_t;

typedef MathVector<StaticVector<double,2> > dvec2_t;
typedef MathVector<StaticVector<double,3> > dvec3_t;
typedef MathVector<StaticVector<double,4> > dvec4_t;
typedef MathVector<StaticVector<double,5> > dvec5_t;
typedef MathVector<StaticVector<double,6> > dvec6_t;

#include "mathvector_operators.h"
#include "mathvector_methods.h"

#endif
