/* Copyright (C) 2026 Interactive Brokers LLC. All rights reserved. This code is subject to the terms
 * and conditions of the IB API Non-Commercial License or the IB API Commercial License, as applicable. */

#pragma once
#ifndef TWS_API_CLIENT_DECIMAL_H
#define TWS_API_CLIENT_DECIMAL_H

#include "platformspecific.h"

#include <climits>
#include <string>

// external functions
extern "C" unsigned long long __bid64_add(unsigned long long, unsigned long long, unsigned int, unsigned int*);
extern "C" unsigned long long __bid64_sub(unsigned long long, unsigned long long, unsigned int, unsigned int*);
extern "C" unsigned long long __bid64_mul(unsigned long long, unsigned long long, unsigned int, unsigned int*);
extern "C" unsigned long long __bid64_div(unsigned long long, unsigned long long, unsigned int, unsigned int*);
extern "C" unsigned long long __bid64_from_string(char*, unsigned int, unsigned int*);
extern "C" void __bid64_to_string(char*, unsigned long long, unsigned int*);
extern "C" double __bid64_to_binary64(unsigned long long, unsigned int, unsigned int*);
extern "C" unsigned long long __binary64_to_bid64(double, unsigned int, unsigned int*);

class DecimalFunctions; // forward declaration

// decimal type - wrapped to prevent numeric conversions
struct TWSAPIDLLEXP Decimal {
private:
    unsigned long long value;
    
    // private constructor - only DecimalFunctions can create
    explicit Decimal(unsigned long long val) : value(val) {}
    
    // make DecimalFunctions a friend so it can access private constructor
    friend class DecimalFunctions;
    
public:
    // default constructor (UNSET_DECIMAL)
    Decimal() : value(ULLONG_MAX) {}
    
    // copy constructor and assignment
    Decimal(const Decimal&) = default;
    Decimal& operator=(const Decimal&) = default;
    
    // DELETE all numeric type constructors to prevent implicit conversion
    Decimal(int) = delete;
    Decimal(long) = delete;
    Decimal(long long) = delete;
    Decimal(unsigned int) = delete;
    Decimal(unsigned long) = delete;
    Decimal(float) = delete;
    Decimal(double) = delete;
    
    // allow implicit conversion TO unsigned long long (for C functions)
    operator unsigned long long() const { return value; }
    
    // comparison operators
    bool operator==(const Decimal& other) const { return value == other.value; }
    bool operator!=(const Decimal& other) const { return value != other.value; }
};

#define UNSET_DECIMAL Decimal()

class TWSAPIDLLEXP DecimalFunctions {
public:
    static Decimal add(Decimal decimal1, Decimal decimal2);
    static Decimal sub(Decimal decimal1, Decimal decimal2);
    static Decimal mul(Decimal decimal1, Decimal decimal2);
    static Decimal div(Decimal decimal1, Decimal decimal2);
    static double decimalToDouble(Decimal decimal);
    static Decimal doubleToDecimal(double d);
    static Decimal stringToDecimal(std::string str);
    static std::string decimalToString(Decimal value);
    static std::string decimalStringToDisplay(Decimal value);
};

#endif
