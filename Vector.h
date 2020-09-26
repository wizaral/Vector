#pragma once
#include <algorithm>
#include <memory>

template <class T>
class Vector final {
    T *m_buffer{nullptr};
    std::size_t m_size{0};
    std::size_t m_capacity{0};

public:
    using value_type = T;
    using iterator = T *;

    Vector() = default;
    ~Vector();

    explicit Vector(std::size_t size);
    Vector(std::size_t size, const T &value);
    Vector(iterator first, iterator last);
    Vector(const std::initializer_list<T> &lst);
    Vector(const Vector<T> &other);

    Vector<T> &operator=(const Vector<T> &other);
    bool operator==(const Vector<T> &other) const;
    bool operator!=(const Vector<T> &other) const;
    bool operator<(const Vector<T> &other) const;
    bool operator>(const Vector<T> &other) const;
    bool operator<=(const Vector<T> &other) const;
    bool operator>=(const Vector<T> &other) const;

    iterator begin() const;
    iterator end() const;

    std::size_t size() const;
    std::size_t capacity() const;
    bool isEmpty() const;

    void resize(std::size_t size, const T &value = T{});
    void reserve(std::size_t size);

    T &operator[](std::size_t index) const;
    T &at(std::size_t index) const;
    T *data() const;

    void push_back(const T &value);
    void pop_back();

    iterator insert(iterator pos, const T &value);
    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);
    void clear();

private:
    T *alloc(std::size_t size);
    void realloc();
};

template <class T>
Vector<T>::~Vector() {
    clear();
    std::free(m_buffer);
}

template <class T>
Vector<T>::Vector(std::size_t size) {
    if (m_buffer = alloc(size); m_buffer == nullptr) {
        throw std::bad_alloc();
    }

    std::uninitialized_default_construct_n(m_buffer, size);
    m_capacity = m_size = size;
}

template <class T>
Vector<T>::Vector(std::size_t size, const T &value) {
    if (m_buffer = alloc(size); m_buffer == nullptr) {
        throw std::bad_alloc();
    }

    std::uninitialized_fill_n(m_buffer, size, value);
    m_capacity = m_size = size;
}

template <class T>
Vector<T>::Vector(iterator first, iterator last) {
    auto size = std::distance(first, last);
    if (m_buffer = alloc(size); m_buffer == nullptr) {
        throw std::bad_alloc();
    }

    std::uninitialized_copy_n(first, size, m_buffer);
    m_capacity = m_size = size;
}

template <class T>
Vector<T>::Vector(const std::initializer_list<T> &list) {
    auto size = list.size();
    if (m_buffer = alloc(size); m_buffer == nullptr) {
        throw std::bad_alloc();
    }

    std::uninitialized_copy_n(list.begin(), size, m_buffer);
    m_capacity = m_size = size;
}

template <class T>
Vector<T>::Vector(const Vector<T> &other) {
    if (m_buffer = alloc(other.m_size); m_buffer == nullptr) {
        throw std::bad_alloc();
    }

    std::uninitialized_copy_n(other.m_buffer, other.m_size, m_buffer);
    m_capacity = m_size = other.m_size;
}

template <class T>
Vector<T> &Vector<T>::operator=(const Vector<T> &other) {
    if (this != &other) {
        if (clear(); m_capacity < other.m_size) {
            reserve(other.m_size);
        }
        std::uninitialized_copy_n(other.m_buffer, other.m_size, m_buffer);
        m_capacity = m_size = other.m_size;
    }
    return *this;
}

template <class T>
bool Vector<T>::operator==(const Vector<T> &other) const {
    return std::equal(begin(), end(), other.begin(), other.end());
}

template <class T>
bool Vector<T>::operator!=(const Vector<T> &other) const {
    return !operator==(other);
}

template <class T>
bool Vector<T>::operator<(const Vector<T> &other) const {
    return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
}

template <class T>
bool Vector<T>::operator>(const Vector<T> &other) const {
    return !operator<(other) && !operator==(other);
}

template <class T>
bool Vector<T>::operator<=(const Vector<T> &other) const {
    return !operator>(other);
}

template <class T>
bool Vector<T>::operator>=(const Vector<T> &other) const {
    return !operator<(other);
}

template <class T>
typename Vector<T>::iterator Vector<T>::begin() const {
    return m_buffer;
}

template <class T>
typename Vector<T>::iterator Vector<T>::end() const {
    return m_buffer + m_size;
}

template <class T>
std::size_t Vector<T>::size() const {
    return m_size;
}

template <class T>
std::size_t Vector<T>::capacity() const {
    return m_capacity;
}

template <class T>
bool Vector<T>::isEmpty() const {
    return m_size == 0;
}

template <class T>
void Vector<T>::resize(std::size_t size, const T &value) {
    if (size > m_size) {
        reserve(size);
        auto diff = size - m_size;
        std::uninitialized_fill_n(end(), diff, value);
        m_size = size;
    } else if (size < m_size) {
        auto diff = m_size - size;
        erase(m_buffer + diff, end());
    }
}

template <class T>
void Vector<T>::reserve(std::size_t size) {
    if (size > m_capacity) {
        auto buffer = alloc(size);
        if (buffer == nullptr) {
            throw std::bad_alloc();
        }

        std::uninitialized_move_n(m_buffer, m_size, buffer);
        std::destroy_n(m_buffer, m_size);
        std::free(m_buffer);
        m_buffer = buffer;
        m_capacity = size;
    }
}

template <class T>
T &Vector<T>::operator[](std::size_t index) const {
    return m_buffer[index];
}

template <class T>
T &Vector<T>::at(std::size_t index) const {
    if (index < m_size) {
        return operator[](index);
    }
    throw std::out_of_range("CBL::Vector: out of range");
}

template <class T>
T *Vector<T>::data() const {
    return m_buffer;
}

template <class T>
void Vector<T>::push_back(const T &value) {
    if (m_size == m_capacity) {
        realloc();
    }
    new (m_buffer + m_size++) T(value);
}

template <class T>
void Vector<T>::pop_back() {
    std::destroy_at(m_buffer + --m_size);
}

template <class T>
typename Vector<T>::iterator Vector<T>::insert(iterator pos, const T &value) {
    auto diff = std::distance(m_buffer, pos);
    if (pos == end()) {
        push_back(value);
    } else {
        if (m_size == m_capacity) {
            realloc();
            pos = m_buffer + diff;
        }
        std::move_backward(pos, end(), end() + 1);
        new (pos) T(value);
        ++m_size;
    }
    return m_buffer + diff;
}

template <class T>
typename Vector<T>::iterator Vector<T>::erase(iterator pos) {
    if (pos == end() - 1) {
        pop_back();
    } else {
        std::destroy_at(pos);
        std::move(pos + 1, end(), pos);
        --m_size;
    }
    return pos;
}

template <class T>
typename Vector<T>::iterator Vector<T>::erase(iterator first, iterator last) {
    auto diff = std::distance(first, last);
    std::destroy(first, last);
    auto temp = end();
    m_size -= diff;

    if (m_size != 0) {
        std::move(last, temp, first);
        return first;
    } else {
        return nullptr;
    }
}

template <class T>
void Vector<T>::clear() {
    std::destroy_n(m_buffer, m_size);
    m_size = 0;
}

template <class T>
T *Vector<T>::alloc(std::size_t size) {
    return static_cast<T *>(std::malloc(size * sizeof(T)));
}

template <class T>
void Vector<T>::realloc() {
    auto capacity = m_capacity != 0 ? m_capacity * 2 : 1;
    auto buffer = alloc(capacity);

    if (buffer == nullptr) {
        throw std::bad_alloc();
    }

    std::uninitialized_move_n(m_buffer, m_size, buffer);
    std::destroy_n(m_buffer, m_size);
    std::free(m_buffer);
    m_buffer = buffer;
    m_capacity = capacity;
}
