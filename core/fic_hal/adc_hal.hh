#include "fic_errors.hh"

#ifndef ADC_HAL_H
#define ADC_HAL_H

class IADC {
    public:
        IADC() = default;
        virtual ~IADC() = default;

        IADC(const IADC&) = delete;            
        IADC& operator=(const IADC&) = delete; 
        IADC(IADC&&) = delete; 

        virtual fic_error_t measure(int &voltage) = 0;
        virtual fic_error_t init() = 0;
};


#endif