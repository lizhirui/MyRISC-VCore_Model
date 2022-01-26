#pragma once
#include "common.h"

namespace component
{
    template<typename T>
    class fifo
    {
        protected:
            T *buffer;
            uint32_t size;
            uint32_t wptr;
            bool wstage;
            uint32_t rptr;
            bool rstage;

        public:
            fifo(uint32_t size);
            ~fifo();
            bool push(T element);
            bool pop(T *element);
            bool is_empty();
            bool is_full();
    };

    template<typename T>
    fifo<T>::fifo(uint32_t size)
    {
        this->size = size;
        buffer = new T[size];
        wptr = 0;
        wstage = false;
        rptr = 0;
        rstage = false;
    }

    template<typename T>
    fifo<T>::~fifo()
    {
        delete[] buffer;
    }

    template<typename T>
    bool fifo<T>::push(T element)
    {
        if(is_full())
        {
            return false;
        }

        buffer[wptr++] = element;

        if(wptr >= size)
        {
            wptr = 0;
            wstage = !wstage;
        }

        return true;
    }

    template<typename T>
    bool fifo<T>::pop(T *element)
    {
        if(is_empty())
        {
            return false;
        }

        *element = buffer[rptr++];

        if(rptr >= size)
        {
            rptr = 0;
            rstage = !rstage;
        }

        return true;
    }

    template<typename T>
    bool fifo<T>::is_empty()
    {
        return (rptr == wptr) && (rstage == wstage);
    }

    template<typename T>
    bool fifo<T>::is_full()
    {
        return (rptr == wptr) && (rstage != wstage);
    }
}