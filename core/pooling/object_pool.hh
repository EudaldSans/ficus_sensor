#ifndef OBJECT_POOL_HH
#define OBJECT_POOL_HH

#include <array>
#include <bitset>

template<typename T, size_t N>
class ObjectPool {
public:
    ObjectPool() {
        for (std::size_t i = 0; i < N; ++i) {
            _available[i] = true;
        }
    }

    T* acquire() {
        for (std::size_t i = 0; i < N; ++i) {
            if (_available[i]) {
                _available[i] = false;
                return &_objects[i];
            }
        }
        return nullptr; // No available objects
    }

    void release(T* obj) {
        size_t index = obj - &_objects[0];
        if (index < N) {
            _available[index] = true;
        }
    }

private:
    std::array<T, N> _objects;
    std::bitset<N> _available;
};

#endif