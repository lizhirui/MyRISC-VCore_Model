#pragma once
#include "common.h"

namespace component
{
    template<typename T>
    class port
    {
        private:
            T value;

        public:
            port(T init_value)
            {
                value = init_value;
            }

            void set(T value)
            {
                this -> value = value;
            }

            T get()
            {
                return this -> value;
            }
    };
}