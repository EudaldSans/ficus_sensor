#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "adc_hal.hh"

#ifndef ADC_H
#define ADC_H

class ADC : public IADC {
    public:
        ADC(adc_channel_t channel, adc_unit_t unit, adc_atten_t attenuation, adc_bitwidth_t bitwidth);        
        ~ADC();
        
        fic_error_t measure(int &voltage) override;
        fic_error_t init() override;

    protected: 
    private: 
        adc_channel_t channel;
        adc_unit_t unit;
        adc_atten_t attenuation;
        adc_bitwidth_t bitwidth;

        adc_oneshot_unit_handle_t adc_handle;
        adc_cali_handle_t adc_cali_handle;

        bool _initialized = false;

        constexpr static char const *TAG = "ADC";
};

#endif