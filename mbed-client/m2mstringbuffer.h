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
    /**
     * Initialize a empty buffer with zero length and zero content.
     */
    inline StringBuffer();

    //
    // This is not implemented on purpose, as the given string may conflict with
    // templated size. Otoh, if we used compile time assert, the overflow
    // could be prevented at compile time.
    //
    // inline StringBuffer(const char *initial_string);

    /**
     * Verify, if the buffer has still room for given amount of bytes.
     * Note: the given size value must include the zero terminator as it is not
     * implicitly taken into account for.
     */
    bool ensure_space(size_t required_size) const;

    /**
     * Append given char to end of string.
     * Return false if the buffer would overflow, true otherwise.
     */
    bool append(char data);

    /**
     * Append given zero terminated string to end of buffer.
     *
     * Return false if the buffer would overflow, true otherwise.
     *
     * Note: the whole string, including the zero terminator must fit
     * to buffer or the append operation is not done and false is returned.
     */
    bool append(const char *data);

    /**
     * Append given block of chars to end of buffer.
     *
     * Return false if the buffer would overflow, true otherwise.
     *
     * Note: the whole string, including the zero terminator must fit
     * to buffer or the append operation is not done and false is returned.
     */
    bool append(const char *data, size_t data_len);

    /**
     * Convert given uint16_t into string representation and add it to the
     * end of buffer.
     *
     * Note: the whole string, including the zero terminator must fit
     * to buffer or the append operation is not done and false is returned.
     */
    bool append_int(uint16_t data);

    /**
     * Get the amount of bytes added to the buffer.
     *
     * Note: the size does not include the terminating zero, so this is
     * functionally equal to strlen().
     */
    inline size_t get_size() const;

    // API functionality copied from m2mstring:

    // find the index of last occurance of given char in string, or negative if not found
    int find_last_of(char search_char) const;

    /**
     * Get a read only pointer to the data.
     */
    inline const char* c_str() const;

    // Add this only if needed
    //inline char* c_str();
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
bool StringBuffer<SIZE>::ensure_space(size_t required_size) const
{
    const size_t space_left = SIZE - _curr_size;

    bool space_available = false;

    if (required_size <= space_left) {

        space_available = true;
    }
    return space_available;
}

template <int SIZE>
bool StringBuffer<SIZE>::append(const char *data)
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
bool StringBuffer<SIZE>::append(const char *data, size_t data_len)
{
    bool space_available = true;
    if (data_len > 0) {
        bool space_available = ensure_space(data_len + 1);
        if (space_available) {
            memcpy(_buff + _curr_size, data, data_len);
            _curr_size += data_len;
            // Todo: should the code actually check, if the data already contained zero or not?
            _buff[_curr_size] = '\0';
            assert(_curr_size < SIZE);
        }
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
bool StringBuffer<SIZE>::append_int(uint16_t data)
{
    // max len of "-9223372036854775808" plus zero termination
    char conv_buff[20+1];

    // re-use the String's functionality, a more optimal version would use snprintf() or int size specific converter
    int len = m2m::itoa_c(data, conv_buff);

    return append(conv_buff, len);
}


template <int SIZE>
int StringBuffer<SIZE>::find_last_of(char search_char) const
{
    int last_index = -1;
    // search from the end of string, return upon first found matching char
    for (int index = _curr_size; index >= 0; index--) {
        if (_buff[index] == search_char) {
            last_index = index;
            break;
        }
    }

    return last_index;
}


template <int SIZE>
inline const char* StringBuffer<SIZE>::c_str() const
{
    return _buff;
}


template <int SIZE>
inline size_t StringBuffer<SIZE>::get_size() const
{
    return _curr_size;
}

#endif // !__STRING_BUFFER_H__
