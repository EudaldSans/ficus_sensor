

#ifndef CHANNEL_LINKING_CONVERSIONS_H
#define CHANNEL_LINKING_CONVERSIONS_H

struct NoConv {
    template <typename T>
    static T apply(T v) { return v; }
};

struct ToFahrenheit {
    template<typename T>
    static T apply(T c) { return (c * 1.8f) + 32.0f; }
};

struct FromFahrenheit {
    static float apply(float f) { return (f - 32.0f) / 1.8f; }
};

template<float operand>
struct Addition {
    static float apply(float v) { return v + operand; }
};

template<float operand>
struct Multiplication {
    static float apply(float v) { return v * operand; }
};

template<float operand>
struct Division {
    static float apply(float v) { return v / operand; }
};

template<typename... Steps>
struct MathChain {
    template <typename T>
    static constexpr T apply(T v) {
        float value = static_cast<float>(v);
        ((value = Steps::apply(value)), ...);
        return static_cast<T>(value);
    }
};

template <typename... Steps>
struct ConverterChain {
    template <typename T>
    static constexpr T apply(T v) {
        ((v = Steps::apply(v)), ...);
        return v;
    }
};

#endif