

#ifndef ADC_HAL_H
#define ADC_HAL_H

class IADC {
    public:
        virtual ~IADC() = default;

        virtual esp_err_t measure(int &voltage) = 0;
        virtual esp_err_t init() = 0;
};


#endif