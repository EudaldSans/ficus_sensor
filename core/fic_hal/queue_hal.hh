#ifndef QUEUE_HAL_HH
#define QUEUE_HAL_HH

template <typename T>
class IQueue {
public:
    virtual ~IQueue() = default;

    virtual bool push(const T& item) = 0;
    virtual bool pop(T& item, bool block, uint16_t timeout_ms) = 0;

    virtual size_t size() const = 0;
    virtual bool full() const = 0;
    virtual bool empty() const = 0;
};

#endif