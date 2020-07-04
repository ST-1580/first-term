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
        if (is_small) {
            for (size_t i = 1; i <= MAX_SIZE; i++) {
                ~small_data[size - i];
            }
        } else {
            long_data->delete_data();
        }
    }

    uint32_t& operator[](size_t id) {
        if (is_small) {
            return small_data[id];
        }
        unshare();
        return long_data->get_elem_link(id);
    }

    uint32_t const& operator[](size_t id) const {
        if (is_small) {
            return small_data[id];
        }
        return long_data->get_elem(id);
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
            long_data = new long_buf(*a.long_data);
        }
        return *this;
    }

    void push_back(uint32_t a) {
        if (is_small) {
            if (size == MAX_SIZE) {
                is_small = false;
                std::vector<uint32_t> new_num;
                for (size_t i = 0; i < MAX_SIZE; i++) {
                    new_num.push_back(small_data[i]);
                }
                new_num.push_back(a);
                long_data = new long_buf(new_num);
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
        while (size < sz) {
            push_back(0);
        }
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
        long_data = long_data->make_data();
    }

private:
    static const size_t MAX_SIZE = 2;
    size_t size;
    bool is_small;
    union {
        uint32_t small_data[MAX_SIZE];
        long_buf* long_data;
    };
};

#endif //BIGINT_BUFFER_H
