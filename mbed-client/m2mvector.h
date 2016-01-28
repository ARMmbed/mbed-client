/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef M2M_VECTOR_H
#define M2M_VECTOR_H

#include <stdio.h>

namespace m2m
{

template <typename ObjectTemplate>

class Vector
{
  public:
    explicit Vector( int init_size = 0)
            : _size( init_size ),
              _index(0),
              _capacity( init_size + MORE_SIZE ){
        _object_template = new ObjectTemplate[ _capacity ];
    }

    Vector(const Vector & rhs ): _object_template(0) {
        operator=(rhs);
    }

    ~Vector() {
        delete [] _object_template;
      _object_template = NULL;
    }

    const Vector & operator=(const Vector & rhs) {
        if(this != &rhs) {
            delete[] _object_template;
            _size = rhs.size();
            _index = rhs._index;
            _capacity = rhs._capacity;

            _object_template = new ObjectTemplate[capacity()];
            for(int k = 0; k < size(); k++) {
                _object_template[k] = rhs._object_template[k];
            }
        }
        return *this;
    }

    void resize(int new_size) {
        if(new_size > _capacity) {
            reserve(new_size * 2 + 1);
        }
        _size = new_size;
    }

    void reserve(int new_capacity) {
        if(new_capacity < _size) {
            return;
        }
        ObjectTemplate *old_array = _object_template;

        _object_template = new ObjectTemplate[new_capacity];
        for(int k = 0; k < _size; k++) {
            _object_template[k] = old_array[k];
        }
        _capacity = new_capacity;
        delete [] old_array;
    }

    ObjectTemplate & operator[](int idx) {
        return _object_template[idx];
    }

    const ObjectTemplate& operator[](int idx) const {
        return _object_template[idx];
    }

    bool empty() const{
        return size() == 0;
    }

    int size() const {
        return _size;
    }

    int capacity() const {
        return _capacity;
    }

    void push_back(const ObjectTemplate& x) {
        if(_size == _capacity) {
            reserve(2 * _capacity + 1);
        }
        _object_template[_index++] = x;
        if(_index >=_capacity) {
            _index--;
        }
        _size++;
    }

    void pop_back() {
        _size--;
        _index--;
    }

    void clear() {
        _size = 0;
        _index = 0;
    }

    const ObjectTemplate& back() const {
        return _object_template[_index- 1];
    }

    typedef ObjectTemplate* iterator;
    typedef const ObjectTemplate* const_iterator;

    iterator begin() {
        return &_object_template[0];
    }

    const_iterator begin() const {
        return &_object_template[0];
    }

    iterator end() {
        return &_object_template[_index];
    }

    const_iterator end() const {
        return &_object_template[_index];
    }

    void erase(int position) {
        if(position <= _size) {
            _object_template[position] = 0;
            _size--;
            if(position < _index) {
                for(int k = position; k < _index; k++) {
                    _object_template[k] = _object_template[k+1];
                }
            }
            _index--;
        }
    }

    enum {
        MORE_SIZE = 32
    };

  private:
    int                 _size;
    int                 _index;
    int                 _capacity;
    ObjectTemplate*     _object_template;
};

} // namespace

#endif // M2M_VECTOR_H
