#ifndef GPIO_HAL_HH
#define GPIO_HAL_HH

class IGPIOInterruptListener {
public:
    /**
     * @brief Callback used when a GPIO detects an event.
     * 
     * @warning Could be called from an ISR, avoid heavy processing during the callback.
     * 
     * @param state Boolean with the current state of the GPIO.
     */
    virtual void gpio_isr_event(const bool state) = 0;
};

class IGPIO {
    public:
        IGPIO() = default;
        IGPIO(IGPIOInterruptListener& listener)
        virtual ~IGPIO() = default;

        // Only One GPIO in the system, does not make sense to copy or move it
        IGPIO(const IGPIO&) = delete;            
        IGPIO& operator=(const IGPIO&) = delete; 
        IGPIO(IGPIO&&) = delete; 

        virtual void set() = 0;
        virtual void reset() = 0;

        virtual bool get_state() = 0;
};

#endif