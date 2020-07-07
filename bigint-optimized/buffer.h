#ifndef BIGINT_BUFFER_H
#define BIGINT_BUFFER_H

#include "long_buf.h"

struct buffer {
    explicit buffer(uint32_t val) : size(1), is_small(true) {
        small_data[0] = val;
    }

    buffer(buffer const& a) : size(a.size), is_small(a.is_small) {
        if (is_small) {
            std::copy(a.small_data, a.small_data + a.size, small_data);
        } else {
            long_data = a.long_data;
            long_data->inc_ref();
        }
    }

    ~buffer() {
        if (!is_small) {
            long_data->delete_data();
        }
    }

    uint32_t& operator[](size_t id) {
        if (is_small) {
            return small_data[id];
        }
        unshare();
        return (*long_data)[id];
    }

    uint32_t const& operator[](size_t id) const {
        if (is_small) {
            return small_data[id];
        }
        return (*long_data)[id];
    }

    uint32_t const& back() const {
        if (is_small) {
            return small_data[size - 1];
        }
        return long_data->back();
    }

    size_t get_size() const {
        return size;
    }

    buffer& operator=(buffer const& a) {
        this->~buffer();
        size = a.size;
        is_small = a.is_small;
        if (a.is_small) {
            std::copy(a.small_data, a.small_data + a.size, small_data);
        } else {
            long_data = a.long_data;
            long_data->inc_ref();
        }
        return *this;
    }

    void push_back(uint32_t a) {
        if (is_small) {
            if (size == MAX_SIZE) {
                is_small = false;
                long_data = new long_buf(small_data, MAX_SIZE);
                long_data->push_back(a);
            } else {
                small_data[size] = a;
            }
        } else {
            unshare();
            long_data->push_back(a);
        }
        size++;
    }

    void pop_back() {
        if (!is_small) {
            unshare();
            long_data->pop_back();
        }
        size--;
    }

    void resize(size_t sz) {
        if (sz > MAX_SIZE) {
            is_small = false;
            if (size <= MAX_SIZE) {
                std::vector<uint32_t> curr(small_data, small_data + size);
                curr.resize(sz);
                long_data = new long_buf(curr);
            } else {
                long_data->resize(sz);
            }
        }
        size = sz;
    }

    void reverse() {
        if (is_small) {
            std::reverse(small_data, small_data + size);
        } else {
            unshare();
            long_data->reverse();
        }
    }

    void unshare() {
        long_data = long_data->make_unique_data();
    }

private:
    static constexpr size_t MAX_SIZE = 2;
    size_t size;
    bool is_small;
    union {
        uint32_t small_data[MAX_SIZE];
        long_buf* long_data;
    };
};

#endif //BIGINT_BUFFER_H
