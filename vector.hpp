#pragma once
#include <algorithm>
#include <memory>

namespace al {

template <class T>
class vector final {
public:
    using value_type = T;

    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using reference = value_type &;
    using rvalue_reference = value_type &&;
    using const_reference = const value_type &;

    using pointer = value_type *;
    using const_pointer = const value_type *;

    using iterator = pointer;
    using const_iterator = const_pointer;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    pointer m_buffer{nullptr};
    size_type m_size{0};
    size_type m_capacity{0};

public:
    constexpr vector() = default;
    ~vector();

    template <class It>
    constexpr vector(It first, It last);

    constexpr explicit vector(size_type size);
    constexpr vector(size_type size, const_reference value);
    constexpr vector(std::initializer_list<T> list);
    constexpr vector(const vector<T> &v);
    constexpr vector(vector<T> &&v) noexcept;

    constexpr vector<T> &operator=(const vector<T> &v) && = delete;
    constexpr vector<T> &operator=(vector<T> &&v) && = delete;

    constexpr vector<T> &operator=(const vector<T> &v) &;
    constexpr vector<T> &operator=(vector<T> &&v) & noexcept;
    constexpr vector<T> &operator=(std::initializer_list<T> list);

    template <class It>
    constexpr vector<T> &assign(It first, It last);

    constexpr vector<T> &assign(size_type size, const_reference value);
    constexpr vector<T> &assign(std::initializer_list<T> list);

    constexpr bool operator==(const vector<T> &v) const;
    constexpr bool operator!=(const vector<T> &v) const;
    constexpr bool operator<(const vector<T> &v) const;
    constexpr bool operator>(const vector<T> &v) const;
    constexpr bool operator<=(const vector<T> &v) const;
    constexpr bool operator>=(const vector<T> &v) const;

    constexpr iterator begin() noexcept;
    constexpr const_iterator begin() const noexcept;
    constexpr iterator end() noexcept;
    constexpr const_iterator end() const noexcept;

    constexpr reverse_iterator rbegin() noexcept;
    constexpr const_reverse_iterator rbegin() const noexcept;
    constexpr reverse_iterator rend() noexcept;
    constexpr const_reverse_iterator rend() const noexcept;

    constexpr const_iterator cbegin() const noexcept;
    constexpr const_reverse_iterator crbegin() const noexcept;
    constexpr const_iterator cend() const noexcept;
    constexpr const_reverse_iterator crend() const noexcept;

    constexpr bool empty() const noexcept;
    constexpr size_type size() const noexcept;
    constexpr size_type max_size() const noexcept;
    constexpr size_type capacity() const noexcept;

    constexpr vector<T> &resize(size_type size);
    constexpr vector<T> &resize(size_type size, const_reference value);

    constexpr vector<T> &shrink_to_fit();
    constexpr vector<T> &reserve(size_type size);

    constexpr reference operator[](size_type index);
    constexpr const_reference operator[](size_type index) const;

    constexpr reference at(size_type index);
    constexpr const_reference at(size_type index) const;

    constexpr reference front();
    constexpr const_reference front() const;

    constexpr reference back();
    constexpr const_reference back() const;

    constexpr pointer data() noexcept;
    constexpr const_pointer data() const noexcept;

    template <class... Args>
    constexpr reference emplace_back(Args &&...args);

    // constexpr vector<T> &push_back(const_reference value);
    constexpr vector<T> &push_back(rvalue_reference value);
    constexpr vector<T> &pop_back();

    template <class... Args>
    constexpr iterator emplace(const_iterator pos, Args &&...args);

    template <class It>
    constexpr iterator insert(const_iterator pos, It first, It last);

    // constexpr iterator insert(const_iterator pos, const_reference value);
    constexpr iterator insert(const_iterator pos, rvalue_reference value);
    constexpr iterator insert(const_iterator pos, size_type size, const_reference value);
    constexpr iterator insert(const_iterator pos, std::initializer_list<T> list);

    constexpr iterator erase(const_iterator pos);
    constexpr iterator erase(const_iterator first, const_iterator last);

    constexpr vector<T> &swap(vector<T> &v) noexcept;
    constexpr vector<T> &clear() noexcept;

private:
    constexpr pointer alloc(size_type size) noexcept;
    constexpr void realloc(size_type size = 0) noexcept;
};

template <class T>
vector<T>::~vector() {
    clear();
    std::free(m_buffer);
}

template <class T>
template <class It>
constexpr vector<T>::vector(It first, It last) {
    m_size = m_capacity = std::distance(first, last);
    m_buffer = alloc(m_size);
    std::uninitialized_copy(first, last, m_buffer);
}

template <class T>
constexpr vector<T>::vector(size_type size)
: m_buffer(alloc(size))
, m_size(size)
, m_capacity(size) {
    std::uninitialized_default_construct_n(m_buffer, size);
}

template <class T>
constexpr vector<T>::vector(size_type size, const_reference value)
: m_buffer(alloc(size))
, m_size(size)
, m_capacity(size) {
    std::uninitialized_fill_n(m_buffer, size, value);
}

template <class T>
constexpr vector<T>::vector(std::initializer_list<T> list) {
    m_capacity = m_size = list.size();
    m_buffer = alloc(m_size);
    std::uninitialized_copy(list.begin(), list.end(), m_buffer);
}

template <class T>
constexpr vector<T>::vector(const vector<T> &v)
: m_buffer(alloc(v.m_size))
, m_size(v.m_size)
, m_capacity(v.m_capacity) {
    std::uninitialized_copy_n(v.m_buffer, v.m_size, m_buffer);
}

template <class T>
constexpr vector<T>::vector(vector<T> &&v) noexcept
: m_buffer(v.m_buffer)
, m_size(v.m_size)
, m_capacity(v.m_capacity) {
    v.m_buffer = nullptr;
    v.m_size = v.m_capacity = 0;
}

template <class T>
constexpr vector<T> &vector<T>::operator=(const vector<T> &v) & {
    if (this != &v) {
        if (clear(); m_capacity < v.m_size) {
            reserve(v.m_size);
        }
        std::uninitialized_copy_n(v.m_buffer, v.m_size, m_buffer);
        m_size = v.m_size;
    }
    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::operator=(vector<T> &&v) & noexcept {
    if (this != &v) {
        clear();

        m_buffer = v.m_buffer;
        m_size = v.m_size;
        m_capacity = v.m_capacity;

        v.m_buffer = nullptr;
        v.m_size = v.m_capacity = 0;
    }
    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::operator=(std::initializer_list<T> list) {
    auto size = list.size();

    if (clear(); m_capacity < size) {
        reserve(size);
    }
    std::uninitialized_copy(list.begin(), list.end(), m_buffer);

    m_size = size;
    return *this;
}

template <class T>
template <class It>
constexpr vector<T> &vector<T>::assign(It first, It last) {
    auto size = std::distance(first, last);

    if (clear(); size > m_capacity) {
        realloc(size);
    }

    m_size = size;
    std::uninitialized_copy(first, last, m_buffer);

    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::assign(size_type size, const_reference value) {
    if (clear(); m_capacity < size) {
        realloc(size);
    }
    std::uninitialized_fill_n(m_buffer, size, value);

    m_size = size;
    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::assign(std::initializer_list<T> list) {
    auto size = list.size();

    if (clear(); size > m_capacity) {
        realloc(size);
    }

    m_size = size;
    std::uninitialized_copy(list.begin(), list.end(), m_buffer);

    return *this;
}

template <class T>
constexpr bool vector<T>::operator==(const vector<T> &v) const {
    return std::equal(begin(), end(), v.begin(), v.end());
}

template <class T>
constexpr bool vector<T>::operator!=(const vector<T> &v) const {
    return !operator==(v);
}

template <class T>
constexpr bool vector<T>::operator<(const vector<T> &v) const {
    return std::lexicographical_compare(begin(), end(), v.begin(), v.end());
}

template <class T>
constexpr bool vector<T>::operator>(const vector<T> &v) const {
    return !operator<(v) && !operator==(v);
    // return std::lexicographical_compare(begin(), end(), v.begin(), v.end(), std::greater<>{});
}

template <class T>
constexpr bool vector<T>::operator<=(const vector<T> &v) const {
    return !operator>(v);
}

template <class T>
constexpr bool vector<T>::operator>=(const vector<T> &v) const {
    return !operator<(v);
}

template <class T>
constexpr typename vector<T>::iterator vector<T>::begin() noexcept {
    return m_buffer;
}

template <class T>
constexpr typename vector<T>::const_iterator vector<T>::begin() const noexcept {
    return m_buffer;
}

template <class T>
constexpr typename vector<T>::iterator vector<T>::end() noexcept {
    return m_buffer + m_size;
}

template <class T>
constexpr typename vector<T>::const_iterator vector<T>::end() const noexcept {
    return m_buffer + m_size;
}

template <class T>
constexpr typename vector<T>::reverse_iterator vector<T>::rbegin() noexcept {
    return std::make_reverse_iterator(end());
}

template <class T>
constexpr typename vector<T>::const_reverse_iterator vector<T>::rbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <class T>
constexpr typename vector<T>::reverse_iterator vector<T>::rend() noexcept {
    return std::make_reverse_iterator(begin());
}

template <class T>
constexpr typename vector<T>::const_reverse_iterator vector<T>::rend() const noexcept {
    return std::make_reverse_iterator(begin());
}

template <class T>
constexpr typename vector<T>::const_iterator vector<T>::cbegin() const noexcept {
    return m_buffer;
}

template <class T>
constexpr typename vector<T>::const_reverse_iterator vector<T>::crbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <class T>
constexpr typename vector<T>::const_iterator vector<T>::cend() const noexcept {
    return m_buffer + m_size;
}

template <class T>
constexpr typename vector<T>::const_reverse_iterator vector<T>::crend() const noexcept {
    return std::make_reverse_iterator(begin());
}

template <class T>
constexpr bool vector<T>::empty() const noexcept {
    return m_size == 0;
}

template <class T>
constexpr typename vector<T>::size_type vector<T>::size() const noexcept {
    return m_size;
}

template <class T>
constexpr typename vector<T>::size_type vector<T>::max_size() const noexcept {
    return std::numeric_limits<size_type>::max() / sizeof(T);
}

template <class T>
constexpr typename vector<T>::size_type vector<T>::capacity() const noexcept {
    return m_capacity;
}

template <class T>
constexpr vector<T> &vector<T>::resize(size_type size) {
    return resize(size, T{});
}

template <class T>
constexpr vector<T> &vector<T>::resize(size_type size, const_reference value) {
    if (size > m_size) {
        reserve(size);
        std::uninitialized_fill_n(end(), size - m_size, value);
        m_size = size;
    } else if (size < m_size) {
        erase(end() - size, end());
    }
    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::shrink_to_fit() {
    auto buffer = alloc(m_size);

    std::uninitialized_move_n(m_buffer, m_size, buffer);
    std::destroy_n(m_buffer, m_size);
    std::free(m_buffer);

    m_buffer = buffer;
    m_capacity = m_size;

    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::reserve(size_type size) {
    if (size > max_size()) {
        throw std::length_error("al::vector: length error");
    }
    if (size > m_capacity) {
        auto buffer = alloc(size);
        std::uninitialized_move_n(m_buffer, m_size, buffer);

        std::destroy_n(m_buffer, m_size);
        std::free(m_buffer);

        m_buffer = buffer;
        m_capacity = size;
    }
    return *this;
}

template <class T>
constexpr typename vector<T>::reference vector<T>::operator[](size_type index) {
    return m_buffer[index];
}

template <class T>
constexpr typename vector<T>::const_reference vector<T>::operator[](size_type index) const {
    return m_buffer[index];
}

template <class T>
constexpr typename vector<T>::reference vector<T>::at(size_type index) {
    if (index < m_size) {
        return operator[](index);
    }
    throw std::out_of_range("al::vector: out of range");
}

template <class T>
constexpr typename vector<T>::const_reference vector<T>::at(size_type index) const {
    if (index < m_size) {
        return operator[](index);
    }
    throw std::out_of_range("al::vector: out of range");
}

template <class T>
constexpr typename vector<T>::pointer vector<T>::data() noexcept {
    return m_buffer;
}

template <class T>
constexpr typename vector<T>::const_pointer vector<T>::data() const noexcept {
    return m_buffer;
}

template <class T>
constexpr typename vector<T>::reference vector<T>::front() {
    return *begin();
}

template <class T>
constexpr typename vector<T>::const_reference vector<T>::front() const {
    return *begin();
}

template <class T>
constexpr typename vector<T>::reference vector<T>::back() {
    return *(end() - 1);
}

template <class T>
constexpr typename vector<T>::const_reference vector<T>::back() const {
    return *(end() - 1);
}

template <class T>
template <class... Args>
constexpr typename vector<T>::reference vector<T>::emplace_back(Args &&...args) {
    if (m_size == m_capacity) {
        realloc();
    }
    ::new (m_buffer + m_size) T(std::forward<Args>(args)...);
    ++m_size;

    return *this;
}

// template <class T>
// constexpr vector<T> &vector<T>::push_back(const_reference value) {
//     if (m_size == m_capacity) {
//         realloc();
//     }
//     ::new (m_buffer + m_size) T(value);
//     ++m_size;

//     return *this;
// }

template <class T>
constexpr vector<T> &vector<T>::push_back(rvalue_reference value) {
    if (m_size == m_capacity) {
        realloc();
    }
    ::new (m_buffer + m_size) T(std::forward<T>(value));
    ++m_size;

    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::pop_back() {
    std::destroy_at(m_buffer + --m_size);
    return *this;
}

template <class T>
template <class... Args>
constexpr typename vector<T>::iterator vector<T>::emplace(const_iterator pos, Args &&...args) {
    auto diff = std::distance(m_buffer, pos);

    if (m_size == m_capacity) {
        realloc();
        pos = m_buffer + diff;
    }
    std::move_backward(pos, end(), end() + 1);
    ::new (pos) T(std::forward<Args>(args)...);

    ++m_size;
    return m_buffer + diff;
}

template <class T>
template <class It>
constexpr typename vector<T>::iterator vector<T>::insert(const_iterator pos, It first, It last) {
    auto diff = std::distance(m_buffer, pos);
    auto size = std::distance(first, last);

    if (size + m_size > m_capacity) {
        realloc(size + m_size);
        pos = m_buffer + diff;
    }

    std::move_backward(pos, end(), end() + size);
    std::uninitialized_copy(first, last, pos);

    m_size += size;
    return m_buffer + diff;
}

// template <class T>
// constexpr typename vector<T>::iterator vector<T>::insert(const_iterator pos, const_reference value) {
//     auto diff = std::distance(m_buffer, pos);

//     if (m_size == m_capacity) {
//         realloc();
//         pos = m_buffer + diff;
//     }
//     std::move_backward(pos, end(), end() + 1);
//     ::new (pos) T(value);

//     ++m_size;
//     return m_buffer + diff;
// }

template <class T>
constexpr typename vector<T>::iterator vector<T>::insert(const_iterator pos, rvalue_reference value) {
    auto diff = std::distance(m_buffer, pos);

    if (m_size == m_capacity) {
        realloc();
        pos = m_buffer + diff;
    }
    std::move_backward(pos, end(), end() + 1);
    ::new (pos) T(std::forward<T>(value));

    ++m_size;
    return m_buffer + diff;
}

template <class T>
constexpr typename vector<T>::iterator vector<T>::insert(const_iterator pos, size_type size, const_reference value) {
    auto diff = std::distance(m_buffer, pos);

    if (m_size + size > m_capacity) {
        realloc(m_size + size);
        pos = m_buffer + diff;
    }
    std::move_backward(pos, end(), end() + size);
    std::uninitialized_fill_n(pos, size, value);

    m_size += size;
    return m_buffer + diff;
}

template <class T>
constexpr typename vector<T>::iterator vector<T>::insert(const_iterator pos, std::initializer_list<T> list) {
    auto diff = std::distance(m_buffer, pos);
    auto size = list.size();

    if (size + m_size > m_capacity) {
        realloc(size + m_size);
        pos = m_buffer + diff;
    }

    std::move_backward(pos, end(), end() + size);
    std::uninitialized_copy(list.begin(), list.end(), pos);

    m_size += size;
    return m_buffer + diff;
}

template <class T>
constexpr typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    std::destroy_at(pos);
    std::move(pos + 1, end(), pos);

    --m_size;
    return pos;
}

template <class T>
constexpr typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
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
constexpr vector<T> &vector<T>::swap(vector<T> &v) noexcept {
    uint8_t bytes[sizeof(vector<T>)];

    memcpy(bytes, this, sizeof(vector<T>));
    memcpy(this, &v, sizeof(vector<T>));
    memcpy(&v, bytes, sizeof(vector<T>));

    return *this;
}

template <class T>
constexpr vector<T> &vector<T>::clear() noexcept {
    std::destroy_n(m_buffer, m_size);
    m_size = 0;
    return *this;
}

template <class T>
constexpr typename vector<T>::pointer vector<T>::alloc(size_type size) noexcept {
    auto res = static_cast<pointer>(std::malloc(size * sizeof(T)));

    if (res == nullptr) {
        throw std::bad_alloc();
    }
    return res;
}

template <class T>
constexpr void vector<T>::realloc(size_type size) noexcept {
    auto capacity = std::max(m_capacity ? m_capacity * 2 : 1, size);
    auto buffer = alloc(capacity);

    std::uninitialized_move_n(m_buffer, m_size, buffer);
    std::destroy_n(m_buffer, m_size);
    std::free(m_buffer);

    m_buffer = buffer;
    m_capacity = capacity;
}

} // namespace al
