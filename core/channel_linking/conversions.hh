

#ifndef CHANNEL_LINKING_CONVERSIONS_H
#define CHANNEL_LINKING_CONVERSIONS_H

struct NoConv {
    template <typename T>
    static T apply(T v) { return v; }
};

/***  Temperature Conversions  ***/
struct ToFahrenheit {
    static float apply(float c) { return (c * 1.8f) + 32.0f; }
};

struct FromFahrenheit {
    static float apply(float f) { return (f - 32.0f) / 1.8f; }
};

/***  Mathematical Conversions  ***/

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

/***  Bitwise Conversions  ***/

template<typename T, int shift>
struct BitShiftLeft {
    static T apply(T v) { return v << shift; }
};

template<typename T, int shift>
struct BitShiftRight {
    static T apply(T v) { return v >> shift; }
};

template<typename T, int mask>
struct BitMask {
    static T apply(T v) { return v & mask; }
};

/*** Logical Conversions  ***/

template<typename T, T value>
struct EqualTo {
    static bool apply(T v) { return v == value; }
};

template<typename T, T value>
struct NotEqualTo {
    static bool apply(T v) { return v != value; }
};

template<typename T, T threshold>
struct GreaterThan {
    static bool apply(T v) { return v > threshold; }
};

template<typename T, T threshold>
struct LessThan {
    static bool apply(T v) { return v < threshold; }
};

/***  Converter Chains  ***/

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