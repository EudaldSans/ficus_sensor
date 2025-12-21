#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

class ADC {
    public:
        ADC(adc_channel_t channel, adc_unit_t unit, adc_atten_t attenuation, adc_bitwidth_t bitwidth);        
        ~ADC();
        
        esp_err_t measure(int &voltage);
        esp_err_t init();

    protected: 
    private: 
        adc_channel_t channel;
        adc_unit_t unit;
        adc_atten_t attenuation;
        adc_bitwidth_t bitwidth;

        adc_oneshot_unit_handle_t adc_handle;
        adc_cali_handle_t adc_cali_handle;

        constexpr static char const *TAG = "ADC";
};
