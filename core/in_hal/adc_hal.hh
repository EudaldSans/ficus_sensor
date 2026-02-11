#include "in_errors.hh"

#ifndef ADC_HAL_H
#define ADC_HAL_H

class IADC {
    public:
        virtual ~IADC() = default;

        virtual in_error_t measure(int &voltage) = 0;
        virtual in_error_t init() = 0;
};


#endif