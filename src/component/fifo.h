#pragma once
#include "common.h"

namespace component
{
    template<typename T>
    class fifo : public if_print_t
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
            bool get_front(T *element);
            bool get_tail(T *element);
            uint32_t get_size();
            bool is_empty();
            bool is_full();
            virtual void print(std::string indent);
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
    bool fifo<T>::get_front(T *element)
    {
        if(this->is_empty())
        {
            return false;
        }

        *element = this->buffer[rptr];
        return true;
    }

    template<typename T>
    bool fifo<T>::get_tail(T *element)
    {
        if(this->is_empty())
        {
            return false;
        }

        *element = this->buffer[(wptr + this->size - 1) % this->size];
        return true;
    }

    template<typename T>
    uint32_t fifo<T>::get_size()
    {
        return this->is_full() ? this->size : ((this->wptr + this->size - this->rptr) % this->size);
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

    template<typename T>
    void fifo<T>::print(std::string indent)
    {
        std::cout << indent << "Item Count = " << this->get_size() << "/" << this->size << std::endl;
        std::cout << indent << "Input:" << std::endl;
        T item;
        if_print_t *if_print;

        if(!this->get_tail(&item))
        {
            std::cout << indent << "\t<Empty>" << std::endl;
        }
        else
        {
            if_print = dynamic_cast<if_print_t *>(&item);
            if_print->print(indent + "\t");
        }

        std::cout << "\tOutput:" << std::endl;

        if(!this->get_front(&item))
        {
            std::cout << indent << "\t<Empty>" << std::endl;
        }
        else
        {
            if_print = dynamic_cast<if_print_t *>(&item);
            if_print->print(indent + "\t");
        }
    }
}