#pragma once
#include "common.h"

namespace component
{
    template<typename T>
    class port : public if_print_t
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
                this->value = value;
            }

            T get()
            {
                return this->value;
            }

            virtual void print(std::string indent)
            {
                if_print_t *if_print = dynamic_cast<if_print_t *>(&this->value);
                if_print->print(indent);
            }

            virtual json get_json()
            {
                if_print_t *if_print = dynamic_cast<if_print_t *>(&this->value);
                return if_print->get_json();
            }
    };
}