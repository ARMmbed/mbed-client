/*
 * Copyright (c) 2016 ARM Limited. All rights reserved.
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
#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include <assert.h>
#include <stddef.h>

template <int SIZE>
class StringBuffer
{
public:
    inline StringBuffer();
    //inline StringBuffer(const char *initial_string);

    inline bool ensure_space(size_t required_size) const;
    inline bool append(char data);
    inline bool append(const char *data);

    inline size_t get_size() const;

    inline const char* c_str() const;
    // inline char* c_str();
private:
    //const size_t _max_size;
    size_t _curr_size;
    char _buff[SIZE];
};

template <int SIZE>
inline StringBuffer<SIZE>::StringBuffer() : _curr_size(0)
{
    // actually a assert_compile() would be better as this is completely a code problem
    assert(SIZE > 0);

    _buff[0] = '\0';
}

template <int SIZE>
inline bool StringBuffer<SIZE>::ensure_space(size_t required_size) const
{
    const size_t space_left = SIZE - _curr_size;

    bool space_available = false;

    if (required_size <= space_left) {

        space_available = true;
    }
    return space_available;
}

template <int SIZE>
inline bool StringBuffer<SIZE>::append(const char *data)
{
    const size_t string_len = strlen(data);
    bool space_available = ensure_space(string_len + 1);
    if (space_available) {
        memcpy(_buff + _curr_size, data, string_len + 1); // copy the zero terminator too
        _curr_size += string_len;
        assert(_curr_size < SIZE);
    }
    return space_available;
}

template <int SIZE>
inline bool StringBuffer<SIZE>::append(char data)
{
    bool space_available = ensure_space(1 + 1); // there must be space for trailing zero too
    if (space_available) {
        _buff[_curr_size++] = data;
        _buff[_curr_size] = '\0';
        assert(_curr_size < SIZE);
    }
    return space_available;
}

template <int SIZE>
inline const char* StringBuffer<SIZE>::c_str() const
{
    return _buff;
}


/*
template <int SIZE>
inline StringBuffer<SIZE>::StringBuffer(const char *initial_string)
{
    size_t string_len = strlen(initial_string);
    strlcpy(_buff, 
}
*/

#endif // !__STRING_BUFFER_H__
