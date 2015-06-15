/*
 * Copyright (c) 2015 ARM. All rights reserved.
 */
#ifndef TICKER_H
#define TICKER_H

namespace mbed
{
typedef uint64_t timestamp_t;

    class Ticker{
    public :
        Ticker() {}
        ~Ticker(){}

        // Ticker
        template<typename T>
        void attach_us(T* tptr, void (T::*mptr)(void), timestamp_t){}

        void detach();
    };
}

#endif // TICKER_H
