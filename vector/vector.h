#include <cstdio>
#include <algorithm>

template <typename T>
struct vector
{
    typedef T* iterator;
    typedef T const* const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const&);                  // O(N) strong
    vector& operator=(vector const& other); // O(N) strong

    ~vector();                              // O(N) nothrow


    T& operator[](size_t i);                // O(1) nothrow
    T const& operator[](size_t i) const;    // O(1) nothrow

    T* data();                              // O(1) nothrow
    T const* data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T& front();                             // O(1) nothrow
    T const& front() const;                 // O(1) nothrow

    T& back();                              // O(1) nothrow
    T const& back() const;                  // O(1) nothrow
    void push_back(T const&);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector&);                     // O(1) nothrow

    iterator begin() {
        return data_;
    }

    iterator end() {
        return data_ + size_;
    }

    const_iterator begin() const {
        return data_;
    }

    const_iterator end() const {
        return data_ + size_;
    }

    iterator insert(const_iterator pos, T const&); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    void push_back_realloc(T const&);
    void new_buffer(std::size_t new_capacity);

private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template<typename T>
vector<T>::vector() = default;

template<typename T>
vector<T>::vector(vector const &other) : vector() {
    T* new_data = nullptr;
    if (other.size_ > 0 ) {
        new_data = static_cast<T*>(operator new(other.size_ * sizeof(T)));
    }
    size_t i;
    try {
        for (i = 0; i < other.size_; i++) {
            new(new_data + i) T(other.data_[i]);
        }
    } catch (...) {
        for (size_t j = 1; j <= i; j++) {
            new_data[i - j].~T();
        }
        operator delete(new_data);
        throw;
    }
    data_ = new_data;
    size_ = other.size_;
    capacity_ = other.size_;
}

template<typename T>
vector<T> &vector<T>::operator=(vector const &other) {
    vector<T> new_vector (other);
    swap(new_vector);
    return *this;
}

template<typename T>
vector<T>::~vector() {
    for (size_t i = 1; i <= size_; i++) {
        data_[size_ - i].~T();
    }
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const &vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
T *vector<T>::data() {
    return data_;
}

template<typename T>
T const *vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T &vector<T>::front() {
    return data_[0];
}

template<typename T>
T const &vector<T>::front() const {
    return data_[0];
}

template<typename T>
T &vector<T>::back() {
    return data_[size_ - 1];
}

template<typename T>
T const &vector<T>::back() const {
    return data_[size_ - 1];
}

template<typename T>
void vector<T>::push_back(const T &value) {
    if (size_ < capacity_) {
        new (data_ + size_) T(value);
    } else {
        push_back_realloc(value);
    }
    size_++;
}

template<typename T>
void vector<T>::pop_back() {
    data_[size_ - 1].~T();
    size_--;
}

template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::reserve(size_t len) {
    if (len > capacity_) {
        new_buffer(len);
    }
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size_ == 0) {
        vector <T> empty;
        swap(empty);
    } else if (size_ < capacity_) {
        T *new_data = static_cast<T*>(operator new(size_ * sizeof(T)));
        for (size_t i = 0; i < size_; i++) {
            new(new_data + i) T(data_[i]);
        }
        for (size_t i = 1; i <= size_; i++) {
            data_[size_ - i].~T();
        }
        operator delete(data_);
        data_ = new_data;
    }
    capacity_ = size_;
}

template<typename T>
void vector<T>::clear() {
    for (size_t i = 1; i <= size_; i++) {
        data_[size_ - i].~T();
    }
    size_ = 0;
}

template<typename T>
void vector<T>::swap(vector & other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator pos, const T &val) {
    ptrdiff_t id = pos - data_;
    ptrdiff_t new_capacity = capacity_;
    if (capacity_ == 0) {
        new_capacity = 1;
    } else if (capacity_ == size_) {
        new_capacity = 2 * capacity_;
    }
    new_buffer(new_capacity);
    new(data_ + size_) T();
    for (size_t i = size_; i > id; i--) {
        data_[i] = data_[i - 1];
    }
    data_[id] = val;
    size_++;
    return data_ + id;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator first, vector::const_iterator last) {
    ptrdiff_t from = first - data_;
    ptrdiff_t to = last - data_;
    ptrdiff_t delta = to - from;
    for (ptrdiff_t i = from; i < size_ - delta; i++) {
        std::swap(data_[i], data_[i + delta]);
    }
    for (ptrdiff_t i = 1; i <= delta; i++) {
        data_[size_ - i].~T();
    }
    size_ -= delta;
    return data_ + to;
}


template<typename T>
void vector<T>::push_back_realloc(const T &val) {
    T temp = val;
    size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
    new_buffer(new_capacity);
    new(data_ + size_) T(temp);
}

template<typename T>
void vector<T>::new_buffer(std::size_t new_capacity) {
    T *new_data = static_cast<T*>(operator new(new_capacity * sizeof(T)));
    size_t i;
    try {
        for (i = 0; i < size_; i++) {
            new(new_data + i) T(data_[i]);
        }
    }
    catch (...) {
        for (size_t j = 1; j <= i; j++) {
            new_data[i - j].~T();
        }
        operator delete(new_data);
        throw;
    }
    this->~vector();
    data_ = new_data;
    capacity_ = new_capacity;
}
