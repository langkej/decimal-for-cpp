#ifndef DEC64_DEC64_HPP
#define DEC64_DEC64_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <iostream>

/**
 * @class dec64
 * @brief A class for representing and performing arithmetic operations on decimal numbers
 *        with a fixed precision, using a coefficient and an exponent.
 */
class dec64 {

private:

    int64_t m_coeff; ///< Coefficient of the decimal number.
    int16_t m_exp;   ///< Exponent of the decimal number (base 10).

#ifdef __SIZEOF_INT128__
    using calc_type = __int128_t; ///< Type used for intermediate calculations (128-bit if available).
#else
    using calc_type = int64_t; ///< Type used for intermediate calculations (64-bit fallback).
#endif

    /**
     * @brief Parses a string view to initialize the dec64 object.
     * @param sv The string view representing the decimal number.
     */
    void from_string_view(std::string_view sv);

    /**
     * @brief Scales the coefficient to a target exponent.
     * @param c The current coefficient.
     * @param current_exp The current exponent.
     * @param target_exp The target exponent.
     * @return The scaled coefficient.
     */
    static calc_type get_scaled_coeff(int64_t c, int16_t current_exp, int16_t target_exp);
    
    /**
     * @brief Creates a dec64 object from a calculation result.
     * @param val The calculated value.
     * @param e The exponent.
     * @return A new dec64 object.
     */
    static dec64 from_calc(calc_type val, int16_t e);

public:

    /**
     * @brief Default constructor, initializes the value to 0.
     */
    dec64() : m_coeff(0), m_exp(0) {}

    /**
     * @brief Constructs a dec64 object from an integer value.
     * @param v The integer value.
     */
    dec64(int v) : m_coeff(v), m_exp(0) {}
    dec64(long v) : m_coeff(v), m_exp(0) {}
    dec64(long long v) : m_coeff(static_cast<int64_t>(v)), m_exp(0) {}

    /**
     * @brief Constructs a dec64 object from a double value.
     * @param v The double value.
     */
    dec64(double v);

    /**
     * @brief Constructs a dec64 object from a string view.
     * @param sv The string view representing the decimal number.
     */
    dec64(std::string_view sv) { from_string_view(sv); }

    /**
     * @brief Constructs a dec64 object from a string.
     * @param s The string representing the decimal number.
     */
    dec64(const std::string& s) { from_string_view(std::string_view(s)); }

    /**
     * @brief Constructs a dec64 object from a C-string.
     * @param s The C-string representing the decimal number.
     */
    dec64(const char* s) { from_string_view(std::string_view(s)); }

    /**
     * @brief Copy constructor.
     */
    dec64(const dec64& o) = default;

    // Assignment operators
    dec64& operator=(const dec64&) = default;
    dec64& operator=(dec64&&) = default;
    dec64& operator=(int v);
    dec64& operator=(long v);
    dec64& operator=(long long v);
    dec64& operator=(double v);
    dec64& operator=(std::string_view sv);
    dec64& operator=(const std::string& s) { return *this = std::string_view(s); }
    dec64& operator=(const char* s) { return *this = std::string_view(s); }

    // Arithmetic operators
    dec64 operator+(const dec64& other) const; ///< Addition operator.
    dec64 operator-(const dec64& other) const; ///< Subtraction operator.
    dec64 operator*(const dec64& other) const; ///< Multiplication operator.
    dec64 operator/(const dec64& other) const; ///< Division operator.

    // Compound assignment operators
    dec64& operator+=(const dec64& other);
    dec64& operator-=(const dec64& other);
    dec64& operator*=(const dec64& other);
    dec64& operator/=(const dec64& other);

    // Comparison operators
    bool operator==(const dec64& other) const;
    bool operator!=(const dec64& other) const { return !(*this == other); }
    bool operator<(const dec64& other) const;
    bool operator>(const dec64& other) const { return other < *this; }
    bool operator<=(const dec64& other) const { return !(*this > other); }
    bool operator>=(const dec64& other) const { return !(*this < other); }

    /**
     * @brief Converts the dec64 object to a string representation.
     * @return The string representation of the decimal number.
     */
    std::string toString() const;

    /**
     * @brief Gets the scale (number of digits after the decimal point).
     * @return The scale of the decimal number.
     */
    int get_scale() const { return (m_exp < 0) ? -m_exp : 0; }

    /**
     * @brief Rescales the decimal number to a new scale.
     * @param new_scale The target scale.
     * @return A new dec64 object with the specified scale.
     */
    dec64 rescale(int new_scale) const;

    /**
     * @brief Outputs the dec64 object to an output stream.
     * @param os The output stream.
     * @param dt The dec64 object.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const dec64& dt);

    /**
     * @brief Computes a hash value for the dec64 object.
     * @return The hash value.
     */
    size_t hash() const;

};

namespace std {

    /**
     * @brief Specialization of std::hash for dec64.
     */
    template <>
    struct hash<dec64> {

        /**
         * @brief Computes a hash value for a dec64 object.
         * @param d The dec64 object.
         * @return The hash value.
         */
        size_t operator()(const dec64& d) const noexcept {
            return d.hash();
        }
        
    };

}

#endif // DEC64_DEC64_HPP