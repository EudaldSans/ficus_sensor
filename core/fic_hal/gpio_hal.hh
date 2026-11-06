#ifndef GPIO_HAL_HH
#define GPIO_HAL_HH

class IGPIO {
public:
    IGPIO() = default;
    virtual ~IGPIO() = default;

    // Only One GPIO in the system, does not make sense to copy or move it
    IGPIO(const IGPIO&) = delete;            
    IGPIO& operator=(const IGPIO&) = delete; 
    IGPIO(IGPIO&&) = delete; 

    virtual void set() = 0;
    virtual void reset() = 0;

    virtual bool get_state() const = 0;
};

#endif