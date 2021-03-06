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
#include "combiobject.h"


CombiObject::CombiObject(ObjectDefinition* object_a, ObjectDefinition* object_b)
{
  m_Objects.clear();
  m_Objects.push_back(object_a);
  m_Objects.push_back(object_b);
  findLowestObjects();

  //  m_ObjectA = object_a;
  //  m_ObjectB = object_b;
  //  findLowestObjects();
}


CombiObject::CombiObject(ObjectDefinition* object_a)
{
  m_Objects.clear();
  m_Objects.push_back(object_a);
  findLowestObjects();
}


void CombiObject::includeObject(ObjectDefinition* object)
{
  m_Objects.push_back(object);
  considerLowestObjectsOf(object);
}


void CombiObject::considerLowestObjectsOf(ObjectDefinition* object)
{
  vector<ObjectDefinition*> your_lowest_objects;
  object->getLowestObjects(your_lowest_objects);
  concatLowestObjects(your_lowest_objects);
}


void CombiObject::concatLowestObjects(vector<ObjectDefinition*>& other_lowest_objects)
{
  // Append elements of other_lowest_objects onto own list, then sort and make unique
  //.. append
  for (size_t i2 = 0; i2 < other_lowest_objects.size(); i2++) {
    m_LowestObjects.push_back(other_lowest_objects[i2]);
  }
  //.. sort
  sort(m_LowestObjects.begin(), m_LowestObjects.end());
  //.. remove duplicates and resize
  vector<ObjectDefinition*>::iterator it;
  it = unique(m_LowestObjects.begin(), m_LowestObjects.end());
  m_LowestObjects.resize(it - m_LowestObjects.begin());
}


void CombiObject::findLowestObjects()
{
  m_LowestObjects.clear();

  // Loop for Objects in this combo.
  // Let objects write into help list and concatenate on m_LowestObjects
  for (size_t i_o = 0; i_o < m_Objects.size(); i_o++) {
    vector<ObjectDefinition*> your_lowest_objects;
    m_Objects[i_o]->getLowestObjects(your_lowest_objects);
    concatLowestObjects(your_lowest_objects);
  }

//  //.. Let object A write directly into own list of "this"
//  m_ObjectA->getLowestObjects(m_LowestObjects);

//  //.. Let object B write into help list and concatenate on m_LowestObjects
//  vector<ObjectDefinition*> your_lowest_objects;
//  m_ObjectB->getLowestObjects(your_lowest_objects);
//  concatLowestObjects(your_lowest_objects);
}


bool CombiObject::isInside(const real& xo, const real& yo, const real& zo)
{
  // Check wether point is inside lowest objects
  for (size_t i_low = 0; i_low < m_LowestObjects.size(); i_low++) {
    bool low_inside = m_LowestObjects[i_low]->isInside(xo, yo, zo);
    //    m_LowObjResponse[i_low] = low_inside;
    m_LowestObjects[i_low]->setKnownInside(low_inside);
  }

  // Own operation
  bool inside = evalBool();

  return inside;
}

