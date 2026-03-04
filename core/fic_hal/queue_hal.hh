#ifndef QUEUE_HAL_HH
#define QUEUE_HAL_HH

template <typename T>
class IQueue {
public:
    virtual ~IQueue() = default;

    virtual bool push(const T& item) = 0;
    virtual bool pop(T& item) = 0;
    virtual size_t count() const = 0;
private: 
    // TODO: add callbacks?
};

#endif