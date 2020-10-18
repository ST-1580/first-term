#include "big_integer.h"

typedef unsigned long long int uint128_t __attribute__ ((mode (TI)));

const uint64_t u32 = static_cast<uint64_t>(UINT32_MAX) + 1;
const big_integer b32 = big_integer(UINT32_MAX) + 1;

big_integer::big_integer() : value(0), sign(false) {

}

big_integer::big_integer(big_integer const& other) : value(other.value), sign(other.sign) {

}

big_integer::big_integer(int a) : value(0), sign(a < 0) {
    if (a == std::numeric_limits<int>::min()) {
        value[0] = 2147483648;
    } else {
        value[0] = (a > 0) ? a : -a;
    }
}

big_integer::big_integer(uint32_t a) : value(a), sign(false) {

}

big_integer::big_integer(std::string const& str) : value(0), sign(false) {
    if (str.empty() || str == "0" || str == "-0") {
        return;
    }
    size_t curr_pos = 0;
    if (str[0] == '-') {
        curr_pos = 1;
    }
    while (curr_pos < str.size()) {
        *this *= 10;
        *this += static_cast<uint32_t>(str[curr_pos] - 48);
        curr_pos++;
    }
    sign = (str[0] == '-');
}

big_integer::~big_integer() = default;

big_integer& big_integer::operator=(big_integer const& other) {
    if (other == *this) {
        return *this;
    }
    value = other.value;
    sign = other.sign;
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    *this = *this + rhs;
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    *this = *this - rhs;
    return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    *this = *this * rhs;
    return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    *this = *this / rhs;
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    *this = *this % rhs;
    return *this;
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    *this = *this & rhs;
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    *this = *this | rhs;
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    *this = *this ^ rhs;
    return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
    *this = *this << rhs;
    return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
    *this = *this >> rhs;
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    if (size() == 1 && value[0] == 0) {
        return *this;
    }
    big_integer to_swap (*this);
    to_swap.sign = !sign;
    return to_swap;
}

big_integer big_integer::operator~() const {
    return -(*this) - 1;
}

big_integer& big_integer::operator++() {
    *this = *this + 1;
    return *this;
}

big_integer big_integer::operator++(int) {
    big_integer old_value = *this;
    *this = *this + 1;
    return old_value;
}

big_integer& big_integer::operator--() {
    *this = *this - 1;
    return *this;
}

big_integer big_integer::operator--(int) {
    big_integer old_value = *this;
    *this = *this - 1;
    return old_value;
}

big_integer operator+(big_integer a, big_integer const& b) {
    if (a.sign != b.sign) {
        return (a.sign) ? b - (-a) : a - (-b);
    }
    size_t max_sz = std::max(a.size(), b.size());
    uint64_t sum;
    uint64_t shift = 0;
    big_integer res;
    for (size_t i = 0; i <= max_sz; i++) {
        res.value.push_back(0);
    }
    for (size_t i = 0; i < max_sz; i++) {
        sum = shift;
        if (i < a.size()) {
            sum += a[i];
        }
        if (i < b.size()) {
            sum += b[i];
        }
        if (sum > UINT32_MAX) {
            shift = 1;
        } else {
            shift = 0;
        }
        res.value[i] = static_cast<uint32_t>(sum % u32);
    }
    res.value[max_sz] = shift;
    res.sign = a.sign;
    res.delete_zero();
    return res;
}

big_integer operator-(big_integer a, big_integer const& b) {
    if (a.sign != b.sign) {
        return (a.sign) ? -(-a + b) : a + (-b);
    }
    if (a.sign) {
        return (-b) - (-a);
    }
    if (a < b) {
        return -(b - a);
    }
    uint32_t shift = 0;
    big_integer res;
    for (size_t i = 0; i < a.size(); i++) {
        res.value.push_back(0);
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (i >= b.size()) {
            if (a[i] >= shift) {
                res.value[i] = a[i] - shift;
                shift = 0;
            } else {
                res.value[i] = UINT32_MAX;
                shift = 1;
            }
        } else if (a[i] >= b[i] + shift) {
            res.value[i] = a[i] - shift - b[i];
            shift = 0;
        } else {
            res.value[i] = UINT32_MAX - (b[i] - a[i]) - shift + 1;
            shift = 1;
        }
    }
    res.delete_zero();
    return res;
}

big_integer operator*(big_integer a, big_integer const& b) {
    if (a == 0 || b == 0) {
        return 0;
    }
    big_integer res;
    for (size_t i = 0; i < a.size() + b.size(); i++) {
        res.value.push_back(0);
    }
    for (size_t i = 0; i < a.size(); i++) {
        uint64_t shift = 0;
        for (size_t j = 0; j < b.size(); j++) {
            uint64_t curr_mul = static_cast<uint64_t>(a[i]) * b[j] + res[i + j] + shift;
            res.value[i + j] = static_cast<uint32_t>(curr_mul % u32);
            shift = curr_mul / u32;
        }
        res.value[i + b.size()] = shift;
    }
    res.sign = a.sign ^ b.sign;
    res.delete_zero();
    return res;
}

big_integer operator/(big_integer a, big_integer const& b) {
    if (b == 1) {
        return a;
    }
    if (b == -1) {
        return -a;
    }
    big_integer first = a;
    big_integer second = b;
    first.sign = second.sign = false;
    big_integer res;
    if (first < second) {
        return 0;
    }
    if (b.size() == 1) {
        uint64_t rest = 0;
        uint64_t curr;
        for (size_t i = 1; i <= a.size(); i++) {
            curr = rest * u32 + a[a.size() - i];
            res.value.push_back(static_cast<uint32_t>(curr / b[0]));
            rest = curr % b[0];
        }
        res.value.reverse();
    } else {
        // Algo from article: Multiple-Length Division Revisited: A Tour of the Minefield
        big_integer dq;
        first.value.push_back(0);
        size_t m = second.size();
        size_t n = first.size();
        res.value.resize(n - m);
        uint32_t qt = 0;
        size_t j = res.size() - 1;
        for (size_t i = m + 1; i <= n; i++) {
            qt = trial(first, second);
            dq = second * qt;
            if (!smaller(first, dq, m + 1)) {
                qt--;
                dq = second * qt;
            }
            res.value[j] = qt;
            difference(first, dq, m + 1);
            if (first.size() != 0) {
                first.value.pop_back();
            }
            j--;
        }
    }
    res.delete_zero();
    res.sign = a.sign ^ b.sign;
    return res;
}

big_integer operator%(big_integer a, big_integer const& b) {
    return a - (a / b) * b;
}

big_integer operator&(big_integer a, big_integer const& b) {
    return bin_operator(a, b, 1);
}

big_integer operator|(big_integer a, big_integer const& b) {
    return bin_operator(a, b, 2);
}

big_integer operator^(big_integer a, big_integer const& b) {
    return bin_operator(a, b, 3);
}

big_integer operator<<(big_integer a, int b) {
    uint32_t shift = 1 << (b % 32);
    for (int i = 0; i < b / 32; i++) {
        a *= b32;
    }
    a *= shift;
    return a;
}

big_integer operator>>(big_integer a, int b) {
    for (int i = 0; i < b / 32; i++) {
        a /= b32;
    }
    uint32_t shift = 1 << (b % 32);
    if (a.sign) {
        a -= shift - 1;
    }
    a /= shift;
    return a;
}

bool operator==(big_integer const& a, big_integer const& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return a.sign == b.sign;
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return !(a == b);
}

bool operator<(big_integer const& a, big_integer const& b) {
    if (a.sign != b.sign) {
        return a.sign;
    }
    if (a.sign) {
        return -a > -b;
    }
    if (a.size() != b.size()) {
        return a.size() < b.size();
    }
    for (size_t i = 1; i <= a.size(); i++) {
        if (a[a.size() - i] != b[a.size() - i]) {
            return a[a.size() - i] < b[a.size() - i];
        }
    }
    return false;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return !(a < b) && a != b;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return !(a < b);
}

std::string to_string(big_integer const& a) {
    if (a == 0) {
        return "0";
    }
    std::string str;
    big_integer curr = a;
    while (curr != 0) {
        big_integer mod10 = curr % 10;
        str += char (mod10[0] + 48);
        curr /= 10;
    }
    str += a.sign ? "-" : "";
    std::reverse(str.begin(), str.end());
    return str;
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}

void big_integer::delete_zero() {
    while (size() > 1 && value.back() == 0) {
        value.pop_back();
    }
}

void big_integer::inverse(size_t sz) {
    for (size_t i = size(); i < sz; i++) {
        value.push_back(0);
    }
    if (sign) {
        sign = false;
        for (size_t i = 0; i < size(); i++) {
           value[i] = ~value[i];
        }
        *this += 1;
    }
}

uint32_t trial(big_integer &a, big_integer const &b) {
    uint128_t x = ((static_cast<uint128_t>(a[a.size() - 1]) << 64) +
                   (static_cast<uint128_t>(a[a.size() - 2]) << 32) +
                   static_cast<uint128_t>(a[a.size() - 3]));
    uint128_t y = ((static_cast<uint128_t>(b[b.size() - 1]) << 32) +
                   static_cast<uint128_t>(b[b.size() - 2]));
    return std::min(static_cast<uint32_t>(x / y), UINT32_MAX);
}

bool smaller(big_integer const &a, big_integer const &b, size_t id) {
    for (size_t i = 1; i <= a.size(); i++) {
        if (id - i < b.size()) {
            if (a[a.size() - i] != b[id - i]) {
                return a[a.size() - i] > b[id - i];
            }
        } else {
            if (a[a.size() - i] != 0) {
                return a[a.size() - i] > 0;
            }
        }
    }
    return true;
}

void difference(big_integer &a, big_integer const &b, size_t id) {
    size_t start = a.size() - id;
    int64_t borrow = 0;
    for (size_t i = 0; i < id; i++) {
        uint32_t val = i < b.size() ? b[i] : 0;
        uint64_t diff = (uint64_t) a[start + i] - val + u32 - borrow;
        a.value[start + i] = diff % u32;
        borrow = 1 - diff / u32;
    }
}

uint32_t bin_op(uint32_t a, uint32_t b, int mode) {
    switch (mode) {
        case 1:
            return a & b;
        case 2:
            return a | b;
        default:
            return a ^ b;
    }
}


big_integer bin_operator(big_integer a, big_integer const& b, int mode) {
    big_integer first_num = a;
    big_integer second_num = b;
    big_integer res;
    size_t max_sz = std::max(a.size(), b.size());
    first_num.inverse(max_sz);
    second_num.inverse(max_sz);
    res.value.resize(max_sz);
    for (size_t i = 0; i < max_sz; i++) {
        res.value[i] = bin_op(first_num[i], second_num[i], mode);
    }
    bool is_inv = bin_op(a.sign, b.sign, mode);
    if (is_inv) {
        res.sign = true;
        res.inverse(max_sz);
        res.sign = true;
    }
    return res;
}

size_t big_integer::size() const {
    return value.get_size();
}

uint32_t big_integer::operator[](const size_t id) const {
    return value[id];
}

