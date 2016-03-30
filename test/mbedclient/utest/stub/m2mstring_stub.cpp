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
#include "m2mstring.h"
#include <string.h> // strlen
#include <stdlib.h> // malloc, realloc
#include <assert.h>
#include <algorithm> // min

namespace m2m {

const String::size_type String::npos = static_cast<size_t>(-1);

char* String::strdup(const char* s)
{
    const size_t len = strlen(s)+1;
    char *p2 = static_cast<char*>(malloc(len));
    memcpy(p2, s, len);
    allocated_ = len;
    size_ = len-1;
    return p2;
}

String::String()
    : p( strdup("") )
{
    _return_value = '\0';
}

String::~String()
{
    free(p);
    p = 0;
}

String::String(const String& s)
    : p(0)
{
    if( &s != NULL ) {
        p = static_cast<char*>(malloc(s.size_ + 1));

        allocated_ = s.size_ + 1;
        size_      = s.size_;
        memcpy(p, s.p, size_ + 1);
    }
    _return_value = '\0';
}

String::String(const char* s)
    : p(strdup(s))
{
    _return_value = '\0';
}

String& String::operator=(const char* s)
{
    if ( p != s ) {
        // s could point into our own string, so we have to allocate a new string
        const size_t len = strlen(s);
        char* copy = (char*) malloc( len + 1);
        memmove(copy, s, len+1); // trailing 0
        free( p );
        p = copy;
        size_ = len;
        allocated_ = len+1;
    }
    return *this;
}

String& String::operator=(const String& s)
{
    return operator=(s.p);
}

String& String::operator+=(const String& s)
{
    if (s.size_ > 0) {
        this->reserve(size_ + s.size_);
        memmove(p+size_, s.p, s.size_+1); // trailing 0
        size_ += s.size_;
    }
    return *this;
}

// since p and s may overlap, we have to copy our own string first
String& String::operator+=(const char* s)
{
    const size_type lens = strlen(s);
    if (lens > 0) {
        if (size_ + lens + 1 <= allocated_) {
            memmove(p+size_, s, lens+1); // trailing 0
            size_ += lens;
        } else {
            String s2( *this );  // copy own data
            s2.reserve(size_ + lens);
            memmove(s2.p+size_, s, lens+1); // trailing 0
            s2.size_ = size_ + lens;
            this->swap( s2 );
        }
    }
    return *this;
}

String& String::operator+=(const char c)
{
    push_back(c);
    return *this;
}

void String::push_back(const char c) {

    if (size_ == allocated_ - 1) {
        size_t more =  (allocated_* 3) / 2; // factor 1.5
        if ( more < 4 ) more = 4;
        reserve( size_ + more );
    }

    p[size_] = c;
    size_++;
    p[size_] = 0;
}

bool String::operator==(const char* s) const
{
    if( s == NULL ) {
        if( p == NULL ) {
            return true;
        }
        return false;
    }
    bool ret = strcmp(p, s);
    return !ret;
}

bool String::operator==(const String& s) const
{
    bool ret = strcmp(p, s.p);
    return !ret;
}

void String::clear()
{
    size_ = 0;
    p[0]  = 0;
}

String String::substr(const size_type pos, size_type length) const
{
    String s;
    const size_type len = size_;

    if ( pos <= len ) {

        size_type remain = len - pos;

        if ( length > remain )
            length = remain;

        s.reserve( length );

        memcpy(s.p, p + pos, length);
        s.p[length] = '\0';
        s.size_ = length;
    }
    return s;
}


// checked access, accessing the NUL at end is allowed
char& String::at(const size_type i)
{
    if ( i <= strlen(p) ) {
        _return_value = p[i];
    } else {
        _return_value = '\0';
    }
    return _return_value;
}

char String::at(const size_type i) const
{
    if ( i <= strlen(p) ) {
        return p[i];
    } else {
        return '\0';
    }
}

String& String::erase(size_type pos, size_type len)
{
    if (len > 0) {

        if ( pos < size_ ) { // user must not remove trailing 0

            size_type s2 = size_;
            size_type remain = s2 - pos - len;

            if (remain > 0) {
                // erase by overwriting
                memmove(p + pos, p + pos + len, remain);
            }

            //if ( remain < 0 ) remain = 0;

            // remove unused space
            this->resize( pos+remain );

        }
    }
    return *this;
}

String& String::append( const char* str, size_type n) {
    if (str && n > 0) {
        size_t lens = strlen(str);
        if (n > lens)
            n = lens;
        size_t newlen = size_ + n;
        this->reserve( newlen );
        memmove(p+size_, str, n); // p and s.p MAY overlap
        p[newlen] = 0; // add NUL termination
        size_ = newlen;
    }
    return *this;
}

String& String::append_raw( const char* str, size_type n) {
    if (str && n > 0) {
        size_t newlen = size_ + n;
        this->reserve( newlen );
        memmove(p+size_, str, n); // p and s.p MAY overlap
        p[newlen] = 0; // add NUL termination
        size_ = newlen;
    }
    return *this;
}

void String::append_int(int param) {

    // max len of "-9223372036854775808" plus zero termination
    char conv_buff[20+1];

    int len = itoa_c(param, conv_buff);
    append_raw(conv_buff, len);
}

int String::compare( size_type pos, size_type len, const String& str ) const {
    int r = -1;
    if (pos <= size_) {
        if ( len > size_ - pos)
            len = size_ - pos; // limit len to available length

        const size_type osize = str.size();
        const size_type len2   = std::min(len, osize);
        r = strncmp( p + pos, str.p, len2);
        if (r==0) // equal so far, now compare sizes
            r = len < osize ? -1 : ( len == osize ? 0 : +1 );
    }
    return r;
}

int String::compare( size_type pos, size_type len, const char* str ) const {
    int r = -1;
    if (pos <= size_) {

        if ( len > size_ - pos)
            len = size_ - pos; // limit len to available length

        const size_type osize = strlen(str);
        const size_type len2   = std::min(len, osize);
        r = strncmp( p + pos, str, len2);
        if (r==0) // equal so far, now compare sizes
            r = len < osize ? -1 : ( len == osize ? 0 : +1 );
    }
    return r;
}

int String::find_last_of(char c) const {
    int r = -1;
    char *v;
    v = strrchr(p,c);
    if (v != NULL) {
        r = 0;
        char* i = p;
        while (v != i) {
            i++;
            r++;
        }
    }
    return r;
}

void String::new_realloc( size_type n) {
    if (n > 0 ) {
        char* pnew = static_cast<char*>(realloc(p, n)); // could return NULL
        if (pnew)
            p = pnew;
    }
}

void String::reserve( const size_type n) {
    if (n >= allocated_ ) {
        this->new_realloc(n + 1);
        allocated_ = n + 1;
    }
}

void String::resize( const size_type n) {
    this->resize( n, 0 );
}

void String::resize( const size_type n, const char c) {
    if (n < size_ ) {
        p[n] = 0;
        size_ = n;
    }
    else if (n >  size_ ) {
        this->reserve( n );
        for (size_type i=size_; i < n; ++i )
            p[i] = c;
        p[n] = 0;
        size_ = n;
    }
}

void String::swap( String& s ) {
    std::swap( allocated_, s.allocated_ );
    std::swap( size_,      s.size_      );
    std::swap( p,          s.p          );
}


// Comparison
bool operator<( const String& s1, const String& s2 ) {
    return strcmp( s1.c_str(), s2.c_str() ) < 0;
}

void reverse(char s[], uint32_t length)
{
    uint32_t i, j;
    char c;

    for (i = 0, j = length-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

uint32_t itoa_c (int64_t n, char s[])
{
    int64_t sign;
    uint32_t i;

    if ((sign = n) < 0)
        n = -n;

    i = 0;

    do {
        s[i++] = n % 10 + '0';
    }
    while ((n /= 10) > 0);

    if (sign < 0)
        s[i++] = '-';

    s[i] = '\0';

    m2m::reverse(s, i);
    return i;
}

uint8_t* String::convert_integer_to_array(int64_t value, uint8_t &size)
{
    uint8_t* buffer;
    if(value < 255) { // 0xFF
        buffer = (uint8_t*)malloc(1);
        *buffer = value;
        size = 1;
    } else if(value < 0xFFFF) { // 0xFFFF
        buffer = (uint8_t*)malloc(2);
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        size = 2;
    } else if(value < 0xFFFFFF) { // 0xFFFFFF 16777215
        buffer = (uint8_t*)malloc(3);
        *buffer++ = value >> 16;
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        buffer--;
        size = 3;
    } else if(value < 0xFFFFFFFF) { // 4294967295
        buffer = (uint8_t*)malloc(4);
        *buffer++ = value >> 24;
        *buffer++ = value >> 16;
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        buffer--;
        buffer--;
        size = 4;
    } else if(value < 0xFFFFFFFFFF) { // 1099511627775
        buffer = (uint8_t*)malloc(5);
        *buffer++ = value >> 32;
        *buffer++ = value >> 24;
        *buffer++ = value >> 16;
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        size = 5;
    } else if(value < 0xFFFFFFFFFFFF) { // 281474976710655
        buffer = (uint8_t*)malloc(6);
        *buffer++ = value >> 40;
        *buffer++ = value >> 32;
        *buffer++ = value >> 24;
        *buffer++ = value >> 16;
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        size = 6;
    } else if(value < 0xFFFFFFFFFFFFFF) { // 72057594037927935
        buffer = (uint8_t*)malloc(7);
        *buffer++ = value >> 48;
        *buffer++ = value >> 40;
        *buffer++ = value >> 32;
        *buffer++ = value >> 24;
        *buffer++ = value >> 16;
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        size = 7;
    } else {
        buffer = (uint8_t*)malloc(8);
        *buffer++ = value >> 56;
        *buffer++ = value >> 48;
        *buffer++ = value >> 40;
        *buffer++ = value >> 32;
        *buffer++ = value >> 24;
        *buffer++ = value >> 16;
        *buffer++ = value >> 8;
        *buffer = value;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        buffer--;
        size = 8;
    }
    return buffer;
}

} // namespace
