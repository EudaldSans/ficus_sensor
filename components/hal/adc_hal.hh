#include "hal/errors.hh"

#ifndef ADC_HAL_H
#define ADC_HAL_H

class IADC {
    public:
        virtual ~IADC() = default;

        virtual IN_error_t measure(int &voltage) = 0;
        virtual IN_error_t init() = 0;
};


#endif