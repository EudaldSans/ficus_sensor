#include "fic_errorshh"

#ifndef ADC_HAL_H
#define ADC_HAL_H

class IADC {
    public:
        virtual ~IADC() = default;

        virtual fic_error_t measure(int &voltage) = 0;
        virtual fic_error_t init() = 0;
};


#endif