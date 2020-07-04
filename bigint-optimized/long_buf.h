#ifndef BIGINT_LONG_BUF_H
#define BIGINT_LONG_BUF_H

#include <iostream>
#include <vector>
#include <algorithm>

struct long_buf {
    long_buf(std::vector<uint32_t> const a) : ref_counter(1), v(a) {

    }

    long_buf(long_buf const& a) : ref_counter(1), v(a.v) {

    }

    ~long_buf() = default;

    long_buf* make_data() {
        if (ref_counter == 1) {
            return this;
        }
        ref_counter--;
        return new long_buf(*this);
    }

    void delete_data() {
        if (ref_counter == 1) {
            delete this;
        } else {
            ref_counter--;
        }
    }

    void inc_ref() {
        ref_counter++;
    }

    uint32_t& get_elem_link(size_t id) {
        return v[id];
    }

    uint32_t const& get_elem(size_t id) {
        return v[id];
    }

    uint32_t const& back() {
        return v.back();
    }

    void push_back(uint32_t val) {
        v.push_back(val);
    }

    void pop_back() {
        v.pop_back();
    }

    void reverse() {
        std::reverse(v.begin(), v.end());
    }

private:
    size_t ref_counter;
    std::vector<uint32_t> v;
};

#endif //BIGINT_LONG_BUF_H
