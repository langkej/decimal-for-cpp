#include "dec64.hpp"

namespace {

    // Lookup table for powers of 10, used for scaling coefficients.
    constexpr int64_t POW10[] = {
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 
        1000000000, 10000000000, 100000000000, 1000000000000, 
        10000000000000, 100000000000000, 1000000000000000, 
        10000000000000000, 100000000000000000, 1000000000000000000
    };

    // Maximum index for the POW10 lookup table.
    constexpr int MAX_POW10 = 18;

}

dec64::dec64(double v) {
    // Converts a double to a string representation and initializes the object.
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "%.15g", v);
    from_string_view(std::string_view(buf, n));
}

dec64& dec64::operator=(int v) {
    // Assigns an integer value to the dec64 object.
    m_coeff = v;
    m_exp = 0;
    return *this;
}

dec64& dec64::operator=(long v) {
    // Assigns a long value to the dec64 object.
    m_coeff = v;
    m_exp = 0;
    return *this;
}

dec64& dec64::operator=(long long v) {
    // Assigns a long long value to the dec64 object.
    m_coeff = static_cast<int64_t>(v);
    m_exp = 0;
    return *this;
}

dec64& dec64::operator=(double v) {
    // Assigns a double value to the dec64 object by converting it to dec64.
    *this = dec64(v);
    return *this;
}

dec64& dec64::operator=(std::string_view sv) {
    // Assigns a string view to the dec64 object by parsing it.
    from_string_view(sv);
    return *this;
}

void dec64::from_string_view(std::string_view sv) {
    // Parses a string view to initialize the coefficient and exponent.
    if(sv.empty()) { m_coeff = 0; m_exp = 0; return; }
    m_coeff = 0;
    m_exp = 0;
    size_t i = 0;
    bool negative = false;

    // Handle optional sign.
    if(sv[i] == '-') { negative = true; ++i; }
    else if(sv[i] == '+') { ++i; }

    bool decimal_found = false;
    int decimal_digits = 0;
    
    // Parse digits and handle the decimal point.
    for(; i < sv.length(); ++i) {
        char c = sv[i];
        if(c == '.') {
            if(decimal_found) break; // Ignore multiple decimal points.
            decimal_found = true;
        } else if(c >= '0' && c <= '9') {
            m_coeff = m_coeff * 10 + (c - '0');
            if (decimal_found) { decimal_digits++; }
        } else { break; }
    }
    
    // Apply the sign and set the exponent.
    if (negative) { m_coeff = -m_coeff; }
    m_exp = static_cast<int16_t>(-decimal_digits);
}

dec64::calc_type dec64::get_scaled_coeff(int64_t c, int16_t current_exp, int16_t target_exp) {
    // Scales the coefficient to match the target exponent.
    if(current_exp == target_exp) return c;
    int diff = current_exp - target_exp;
    if(diff < 0) return c; // No scaling needed if the target exponent is smaller.
    
    // Use the lookup table for efficient scaling.
    calc_type res = c;
    if(diff <= MAX_POW10) { res *= POW10[diff]; }
    else { for(int k=0; k<diff; ++k) { res *= 10; } } // Fallback for large differences (unlikely to occur).
    return res;
}

dec64 dec64::from_calc(calc_type val, int16_t e) {
    // Creates a dec64 object from a calculation result, adjusting the coefficient and exponent.
    dec64 res;
    res.m_exp = e;
    
    // Normalize the coefficient to fit within the range of int64_t.
    while(val > std::numeric_limits<int64_t>::max() || val < std::numeric_limits<int64_t>::min()) {
        val /= 10;
        res.m_exp++;
    }
    res.m_coeff = static_cast<int64_t>(val);
    return res;
}

dec64 dec64::operator+(const dec64& other) const {
    // Adds two dec64 objects, aligning their exponents.
    int16_t target_exp = std::min(m_exp, other.m_exp);
    calc_type c1 = get_scaled_coeff(m_coeff, m_exp, target_exp);
    calc_type c2 = get_scaled_coeff(other.m_coeff, other.m_exp, target_exp);
    return from_calc(c1 + c2, target_exp);
}

dec64 dec64::operator-(const dec64& other) const {
    // Subtracts two dec64 objects, aligning their exponents.
    int16_t target_exp = std::min(m_exp, other.m_exp);
    calc_type c1 = get_scaled_coeff(m_coeff, m_exp, target_exp);
    calc_type c2 = get_scaled_coeff(other.m_coeff, other.m_exp, target_exp);
    return from_calc(c1 - c2, target_exp);
}

dec64 dec64::operator*(const dec64& other) const {
    // Multiplies two dec64 objects.
    calc_type c = static_cast<calc_type>(m_coeff) * other.m_coeff;
    int16_t e = m_exp + other.m_exp;
    return from_calc(c, e);
}

dec64 dec64::operator/(const dec64& other) const {
    // Divides two dec64 objects, with scaling to maintain precision.
    if(other.m_coeff == 0) throw std::overflow_error("Division by zero");
    constexpr int DIV_SCALE = 10; 
    calc_type numerator = static_cast<calc_type>(m_coeff);
    for(int i = 0; i < DIV_SCALE; ++i) numerator *= 10;
    calc_type res = numerator / other.m_coeff;
    int16_t final_exp = m_exp - other.m_exp - DIV_SCALE;
    return from_calc(res, final_exp);
}

dec64& dec64::operator+=(const dec64& other) {
    *this = *this + other;
    return *this;
}

dec64& dec64::operator-=(const dec64& other) {
    *this = *this - other;
    return *this;
}

dec64& dec64::operator*=(const dec64& other) {
    *this = *this * other;
    return *this;
}

dec64& dec64::operator/=(const dec64& other) {
    *this = *this / other;
    return *this;
}

bool dec64::operator==(const dec64& other) const {
    // Compares two dec64 objects for equality, aligning their exponents.
    if(m_exp == other.m_exp) return m_coeff == other.m_coeff;
    int16_t target = std::min(m_exp, other.m_exp);
    calc_type c1 = get_scaled_coeff(m_coeff, m_exp, target);
    calc_type c2 = get_scaled_coeff(other.m_coeff, other.m_exp, target);
    return c1 == c2;
}

bool dec64::operator<(const dec64& other) const {
    // Compares two dec64 objects for less-than, aligning their exponents.
    int16_t target = std::min(m_exp, other.m_exp);
    calc_type c1 = get_scaled_coeff(m_coeff, m_exp, target);
    calc_type c2 = get_scaled_coeff(other.m_coeff, other.m_exp, target);
    return c1 < c2;
}

std::string dec64::toString() const {
    // Converts the dec64 object to a string representation.
    std::string s = std::to_string(abs(m_coeff));
    if(m_exp >= 0) { s.append(m_exp, '0'); }
    else {
        int decimals = -m_exp;
        int len = static_cast<int>(s.length());
        
        if (len <= decimals) {
            std::string prefix = "0.";
            prefix.append(decimals - len, '0');
            s = prefix + s;
        } else {
            s.insert(len - decimals, ".");
        }
    }
    if(m_coeff < 0) { s.insert(0, "-"); }
    return s;
}

dec64 dec64::rescale(int new_scale) const {
    // Rescales the dec64 object to a new scale, adjusting the coefficient and exponent.
    int16_t target_exp = -new_scale;
    if(m_exp == target_exp) return *this;
    calc_type val = m_coeff;
    if(m_exp > target_exp) {
        int diff = m_exp - target_exp;
        for(int i=0; i<diff; ++i) val *= 10;
    } else {
        int diff = target_exp - m_exp;
        calc_type div = 1;
        for(int i=0; i<diff; ++i) div *= 10;
        calc_type rem = val % div;
        val /= div;
#if defined(__SIZEOF_INT128__)
    if((rem < 0 ? -rem : rem) * 2 >= div) { val += (val > 0) ? 1 : -1; }
#else
    if(std::abs(rem) * 2 >= div) { val += (val > 0) ? 1 : -1; }
#endif
    }
    return from_calc(val, target_exp);
}

std::ostream& operator<<(std::ostream& os, const dec64& dt) {
    // Outputs the dec64 object to a stream.
    os << dt.toString();
    return os;
}

size_t dec64::hash() const {
    // Computes a hash value for the dec64 object.
    if(m_coeff == 0) { return std::hash<int64_t>{}(0); }
    int64_t temp_coeff = m_coeff;
    int16_t temp_exp   = m_exp;
    while(temp_coeff % 10 == 0) {
        temp_coeff /= 10;
        temp_exp++;
    }
    size_t h1 = std::hash<int64_t>{}(temp_coeff);
    size_t h2 = std::hash<int16_t>{}(temp_exp);
    return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));

}
