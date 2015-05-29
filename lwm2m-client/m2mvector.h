/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef M2M_VECTOR_H
#define M2M_VECTOR_H

#include <stdio.h>

namespace m2m
{

template <typename Object>

class Vector
{
  public:

    explicit Vector( int initSize = 0)
            : theSize( initSize ),
              index(0),
              theCapacity( initSize + SPARE_CAPACITY )
    { objects = new Object[ theCapacity ]; }

    Vector(const Vector & rhs ): objects(0)
    {operator=(rhs); }

    ~Vector()
    { delete [] objects;
      objects = NULL;}

    const Vector & operator=(const Vector & rhs)
    {
        if(this != &rhs)
        {
            delete[] objects;
            theSize = rhs.size();
            index = rhs.index;
            theCapacity = rhs.theCapacity;

            objects = new Object[capacity()];
            for(int k = 0; k < size(); k++)
                objects[k] = rhs.objects[k];
        }

        return *this;
    }

    void resize(int newSize)
    {
        if(newSize > theCapacity)
            reserve(newSize * 2 + 1);
        theSize = newSize;
    }

    void reserve(int newCapacity)
    {
        if(newCapacity < theSize)
            return;

        Object *oldArray = objects;

        objects = new Object[newCapacity];
        for(int k = 0; k < theSize; k++)
            objects[k] = oldArray[k];

        theCapacity = newCapacity;

        delete [] oldArray;
    }

    Object & operator[](int idx)
    { return objects[idx]; }
    
    const Object& operator[](int idx) const
    { return objects[idx]; }

    bool empty() const
    { return size() == 0; }
    
    int size() const
    { return theSize; }
    
    int capacity() const
    { return theCapacity; }

    void push_back(const Object& x)
    {        
        if(theSize == theCapacity)
            reserve(2 * theCapacity + 1);
        objects[index++] = x;
        theSize++;
    }

    void pop_back()
    {
        theSize--;
        index--;
    }

    void clear()
    {
        theSize = 0;
        index = 0;
    }

    const Object& back() const
    { return objects[index- 1]; }

    typedef Object* iterator;
    typedef const Object* const_iterator;

    iterator begin()
    { return &objects[0]; }
    const_iterator begin() const
    { return &objects[0]; }

    iterator end()
    { return &objects[index]; }
    const_iterator end() const
    { return &objects[index]; }

    void erase(int position)
    {
        if(position <= theSize) {
            objects[position] = 0;
            theSize--;
        }
    }

    enum { SPARE_CAPACITY = 32 };
  private:
    int theSize;
    int index;
    int theCapacity;
    Object* objects;
};

} // namespace

#endif // M2M_VECTOR_H
