#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <string>
#include <limits>
#include <algorithm>

struct big_integer
{
private:
    void delete_zero();
    void inverse(size_t sz);
    friend uint32_t trial(big_integer &a, big_integer const &b);
    friend bool smaller(big_integer const &a, big_integer const &b, size_t index);
    friend void difference(big_integer &a, big_integer const &b, size_t index);
    friend big_integer bin_operator(big_integer a, big_integer const& b, int mode);
    size_t size() const;
    uint32_t operator[](const size_t id) const;
public:
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    big_integer(uint32_t a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend big_integer operator+(big_integer a, big_integer const& b);
    friend big_integer operator-(big_integer a, big_integer const& b);
    friend big_integer operator*(big_integer a, big_integer const& b);
    friend big_integer operator/(big_integer a, big_integer const& b);
    friend big_integer operator%(big_integer a, big_integer const& b);

    friend big_integer operator&(big_integer a, big_integer const& b);
    friend big_integer operator|(big_integer a, big_integer const& b);
    friend big_integer operator^(big_integer a, big_integer const& b);

    friend big_integer operator<<(big_integer a, int b);
    friend big_integer operator>>(big_integer a, int b);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);
    friend std::ostream& operator<<(std::ostream& s, big_integer const& a);
private:
    std::vector<uint32_t> value;
    bool sign;
};

#endif // BIG_INTEGER_H