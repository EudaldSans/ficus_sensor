

#ifndef CHANNEL_LINKING_CONVERSIONS_H
#define CHANNEL_LINKING_CONVERSIONS_H

template <typename T>
class IConversion {
    public:
        virtual ~IConversion() = default; 
        
        T convert(T value) {
            T converted_value = perform_conversion(value);

            return converted_value;
        }

    protected:
        virtual T perform_conversion(T value) {return value;}
};

template <typename T>
class AdditionConversion : public IConversion<T> {
    public:
        AdditionConversion(T operand) : _operand(operand) {}
    
    protected:
        T perform_conversion(T value) override {return value + _operand;}
    
    private:
        T _operand;
};

template <typename T>
class ToFahrenheitConversion : public IConversion<T> {   
    protected:
        T perform_conversion(T value) override {
            return (value * 1.8) + 32;
        }
};

template <typename T>
class ToCelsiusConversion : public IConversion<T> {
    protected:
        T perform_conversion(T value) override {
            return (value - 32) / 1.8;
        }
};

#endif