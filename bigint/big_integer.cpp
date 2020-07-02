#include "big_integer.h"

typedef unsigned long long int uint128_t __attribute__ ((mode (TI)));

big_integer::big_integer() : value(1), sign(false) {

}

big_integer::big_integer(big_integer const& other) {
    sign = other.sign;
    value = other.value;
}

big_integer::big_integer(int a) : value(1), sign(a < 0){
    if (a == std::numeric_limits<int>::min()) {
        value[0] = 2147483648;
    } else {
        value[0] = (a > 0) ? a : -a;
    }
}

big_integer::big_integer(uint32_t a) : value(1), sign(false) {
    value[0] = a;
}

big_integer::big_integer(std::string const& str) : value(1), sign(false) {
    if (str.empty() || str == "0" || str == "-0") {
        return;
    }
    size_t curr_pos = 0;
    if (str[0] == '-') {
        curr_pos = 1;
    }
    while (curr_pos < str.size()) {
        *this *= 10;
        *this += (uint32_t) (str[curr_pos] - 48);
        curr_pos++;
    }
    sign = (str[0] == '-');
}

big_integer::~big_integer() {
    value.clear();
}

big_integer& big_integer::operator=(big_integer const& other) {
    big_integer to_swap (other);
    std::swap(sign, to_swap.sign);
    std::swap(value, to_swap.value);
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
    if (value.size() == 1 && value[0] == 0) {
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
        if (a.sign) {
            return b - (-a);
        }
        return a - (-b);
    }
    size_t max_sz = std::max(a.value.size(), b.value.size());
    uint64_t sum;
    uint64_t shift = 0;
    big_integer res;
    uint64_t u32 = (uint64_t) UINT32_MAX + 1;
    for (size_t i = 0; i <= max_sz; i++) {
        res.value.push_back(0);
    }
    for (size_t i = 0; i < max_sz; i++) {
        sum = shift;
        if (i < a.value.size()) {
            sum += a.value[i];
        }
        if (i < b.value.size()) {
            sum += b.value[i];
        }
        if (sum > UINT32_MAX) {
            shift = 1;
        } else {
            shift = 0;
        }
        res.value[i] = (uint32_t) (sum % u32);
    }
    res.value[max_sz] = shift;
    res.sign = a.sign;
    res.delete_zero();
    return res;
}

big_integer operator-(big_integer a, big_integer const& b) {
    if (a.sign != b.sign) {
        if (a.sign) {
            return -(-a + b);
        }
        return a + (-b);
    }
    if (a.sign) {
        return (-b) - (-a);
    }
    if (a < b) {
        return -(b - a);
    }
    uint32_t shift = 0;
    big_integer res;
    for (size_t i = 0; i < a.value.size(); i++) {
        res.value.push_back(0);
    }
    for (size_t i = 0; i < a.value.size(); i++) {
        if (i >= b.value.size()) {
            if (a.value[i] >= shift) {
                res.value[i] = a.value[i] - shift;
                shift = 0;
            } else {
                res.value[i] = UINT32_MAX;
                shift = 1;
            }
        } else if (a.value[i] >= b.value[i] + shift) {
            res.value[i] = a.value[i] - shift - b.value[i];
            shift = 0;
        } else {
            res.value[i] = UINT32_MAX - (b.value[i] - a.value[i]) - shift + 1;
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
    for (size_t i = 0; i < a.value.size() + b.value.size(); i++) {
        res.value.push_back(0);
    }
    uint64_t u32 = (uint64_t) UINT32_MAX + 1;
    for (size_t i = 0; i < a.value.size(); i++) {
        uint64_t shift = 0;
        for (size_t j = 0; j < b.value.size(); j++) {
            uint64_t curr_mul = (uint64_t) a.value[i] * b.value[j] + res.value[i + j] + shift;
            res.value[i + j] = (uint32_t) curr_mul % u32;
            shift = curr_mul / u32;
        }
        res.value[i + b.value.size()] = shift;
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
    uint64_t u32 = (uint64_t) UINT32_MAX + 1;
    if (b.value.size() == 1) {
        uint64_t rest = 0;
        uint64_t curr;
        for (size_t i = 1; i <= a.value.size(); i++) {
            curr = rest * u32 + a.value[a.value.size() - i];
            res.value.push_back((uint32_t) (curr / b.value[0]));
            rest = curr % b.value[0];
        }
        std::reverse(res.value.begin(), res.value.end());
    } else {
        // Algo from article: Multiple-Length Division Revisited: A Tour of the Minefield
        big_integer dq;
        first.value.push_back(0);
        size_t m = second.value.size();
        size_t n = first.value.size();
        res.value.resize(n - m);
        uint32_t qt = 0;
        size_t j = res.value.size() - 1;
        for (size_t i = m + 1; i <= n; i++) {
            qt = trial(first, second);
            dq = second * qt;
            if (!smaller(first, dq, m + 1)) {
                qt--;
                dq = second * qt;
            }
            res.value[j] = qt;
            difference(first, dq, m + 1);
            if (first.value.size() != 0) {
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
    uint32_t shift = 1 << b % 32;
    big_integer u32 = big_integer(UINT32_MAX) + 1;
    for (int i = 0; i < b / 32; i++) {
        a *= u32;
    }
    a *= shift;
    return a;
}

big_integer operator>>(big_integer a, int b) {
    big_integer u32 = big_integer(UINT32_MAX) + 1;
    for (int i = 0; i < b / 32; i++) {
        a /= u32;
    }
    if (a.sign) {
        a -= (uint32_t) (1 << b % 32) - 1;
    }
    uint32_t shift = 1 << b % 32;
    a /= shift;
    return a;
}

bool operator==(big_integer const& a, big_integer const& b) {
    if (a.value.size() != b.value.size()) {
        return false;
    }
    for (size_t i = 0; i < a.value.size(); i++) {
        if (a.value[i] != b.value[i]) {
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
    if (a.value.size() != b.value.size()) {
        return a.value.size() < b.value.size();
    }
    for (size_t i = 1; i <= a.value.size(); i++) {
        if (a.value[a.value.size() - i] != b.value[a.value.size() - i]) {
            return a.value[a.value.size() - i] < b.value[a.value.size() - i];
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
        str += char (mod10.value[0] + 48);
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
    while (value.size() > 1 && value.back() == 0) {
        value.pop_back();
    }
}

void big_integer::inverse(size_t sz) {
    for (size_t i = value.size(); i < sz; i++) {
        value.push_back(0);
    }
    if (sign) {
        sign = false;
        for (size_t i = 0; i < value.size(); i++) {
            value[i] = ~value[i];
        }
        *this += 1;
    }
}

uint32_t trial(big_integer &a, big_integer const &b) {
    uint128_t x = (((uint128_t) a.value[a.value.size() - 1] << 64) +
                   ((uint128_t) a.value[a.value.size() - 2] << 32) +
                   ((uint128_t) a.value[a.value.size() - 3]));
    uint128_t y = (((uint128_t) b.value[b.value.size() - 1] << 32) +
                   (uint128_t) b.value[b.value.size() - 2]);
    return std::min((uint32_t) (x / y), UINT32_MAX);
}

bool smaller(big_integer const &a, big_integer const &b, size_t id) {
    for (size_t i = 1; i <= a.value.size(); i++) {
        if (id - i < b.value.size()) {
            if (a.value[a.value.size() - i] != b.value[id - i]) {
                return a.value[a.value.size() - i] > b.value[id - i];
            }
        } else {
            if (a.value[a.value.size() - i] != 0) {
                return a.value[a.value.size() - i] > 0;
            }
        }
    }
    return true;
}

void difference(big_integer &a, big_integer const &b, size_t id) {
    size_t start = a.value.size() - id;
    int64_t borrow = 0;
    uint64_t u32 = (uint64_t) UINT32_MAX + 1;
    for (size_t i = 0; i < id; i++) {
        uint32_t val = i < b.value.size() ? b.value[i] : 0;
        uint64_t diff = (uint64_t) a.value[start + i] - val + u32 - borrow;
        a.value[start + i] = diff % u32;
        borrow = 1 - diff / u32;
    }
}

uint32_t bin_op(uint32_t a, uint32_t b, int mode) {
    if (mode == 1) {
        return a & b;
    } else if (mode == 2) {
        return a | b;
    } else {
        return a ^ b;
    }
}


big_integer bin_operator(big_integer a, big_integer const& b, int mode) {
    big_integer first_num = a;
    big_integer second_num = b;
    big_integer res;
    size_t max_sz = std::max(a.value.size(), b.value.size());
    first_num.inverse(max_sz);
    second_num.inverse(max_sz);
    res.value.resize(max_sz);
    for (size_t i = 0; i < max_sz; i++) {
        res.value[i] = bin_op(first_num.value[i], second_num.value[i], mode);
    }
    bool is_inv = bin_op(a.sign, b.sign, mode);
    if (is_inv) {
        res.sign = true;
        res.inverse(max_sz);
        res.sign = true;
    }
    return res;
}