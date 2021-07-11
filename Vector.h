#pragma once
#include <algorithm>
#include <memory>

namespace al {

template <class T>
class Vector final {
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
    constexpr Vector() = default;
    constexpr ~Vector();

    template <class It>
    constexpr Vector(It first, It last);

    constexpr explicit Vector(size_type size);
    constexpr Vector(size_type size, const_reference value);
    constexpr Vector(std::initializer_list<T> list);
    constexpr Vector(const Vector<T> &v);
    constexpr Vector(Vector<T> &&v) noexcept;

    constexpr Vector<T> &operator=(const Vector<T> &v) && = delete;
    constexpr Vector<T> &operator=(Vector<T> &&v) && = delete;

    constexpr Vector<T> &operator=(const Vector<T> &v) &;
    constexpr Vector<T> &operator=(Vector<T> &&v) & noexcept;
    constexpr Vector<T> &operator=(std::initializer_list<T> list);

    template <class It>
    constexpr Vector<T> &assign(It first, It last);

    constexpr Vector<T> &assign(size_type size, const_reference value);
    constexpr Vector<T> &assign(std::initializer_list<T> list);

    constexpr bool operator==(const Vector<T> &v) const;
    constexpr bool operator!=(const Vector<T> &v) const;
    constexpr bool operator<(const Vector<T> &v) const;
    constexpr bool operator>(const Vector<T> &v) const;
    constexpr bool operator<=(const Vector<T> &v) const;
    constexpr bool operator>=(const Vector<T> &v) const;

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

    constexpr Vector<T> &resize(size_type size);
    constexpr Vector<T> &resize(size_type size, const_reference value);

    constexpr Vector<T> &shrink_to_fit();
    constexpr Vector<T> &reserve(size_type size);

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

    constexpr Vector<T> &push_back(const_reference value);
    constexpr Vector<T> &push_back(rvalue_reference value);
    constexpr Vector<T> &pop_back();

    template <class... Args>
    constexpr iterator emplace(const_iterator pos, Args &&...args);

    template <class It>
    constexpr iterator insert(const_iterator pos, It first, It last);

    constexpr iterator insert(const_iterator pos, const_reference value);
    constexpr iterator insert(const_iterator pos, rvalue_reference value);
    constexpr iterator insert(const_iterator pos, size_type size, const_reference value);
    constexpr iterator insert(const_iterator pos, std::initializer_list<T> list);

    constexpr iterator erase(const_iterator pos);
    constexpr iterator erase(const_iterator first, const_iterator last);

    constexpr Vector<T> &swap(Vector<T> &v) noexcept;
    constexpr Vector<T> &clear() noexcept;

private:
    constexpr pointer alloc(size_type size) noexcept;
    constexpr void realloc(size_type size = 0) noexcept;
};

template <class T>
constexpr Vector<T>::~Vector() {
    clear();
    std::free(m_buffer);
}

template <class T>
template <class It>
constexpr Vector<T>::Vector(It first, It last) {
    m_size = m_capacity = std::distance(first, last);
    m_buffer = alloc(m_size);
    std::uninitialized_copy(first, last, m_buffer);
}

template <class T>
constexpr Vector<T>::Vector(size_type size)
: m_buffer(alloc(size))
, m_size(size)
, m_capacity(size) {
    std::uninitialized_default_construct_n(m_buffer, size);
}

template <class T>
constexpr Vector<T>::Vector(size_type size, const_reference value)
: m_buffer(alloc(size))
, m_size(size)
, m_capacity(size) {
    std::uninitialized_fill_n(m_buffer, size, value);
}

template <class T>
constexpr Vector<T>::Vector(std::initializer_list<T> list) {
    m_capacity = m_size = list.size();
    m_buffer = alloc(m_size);
    std::uninitialized_copy(list.begin(), list.end(), m_buffer);
}

template <class T>
constexpr Vector<T>::Vector(const Vector<T> &v)
: m_buffer(alloc(v.m_size))
, m_size(v.m_size)
, m_capacity(v.m_capacity) {
    std::uninitialized_copy_n(v.m_buffer, v.m_size, m_buffer);
}

template <class T>
constexpr Vector<T>::Vector(Vector<T> &&v) noexcept
: m_buffer(v.m_buffer)
, m_size(v.m_size)
, m_capacity(v.m_capacity) {
    v.m_buffer = nullptr;
    v.m_size = v.m_capacity = 0;
}

template <class T>
constexpr Vector<T> &Vector<T>::operator=(const Vector<T> &v) & {
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
constexpr Vector<T> &Vector<T>::operator=(Vector<T> &&v) & noexcept {
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
constexpr Vector<T> &Vector<T>::operator=(std::initializer_list<T> list) {
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
constexpr Vector<T> &Vector<T>::assign(It first, It last) {
    clear();
    auto size = std::distance(first, last);

    if (size > m_capacity) {
        realloc(size);
    }

    m_size = size;
    std::uninitialized_copy(first, last, m_buffer);

    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::assign(size_type size, const_reference value) {
    if (clear(); m_capacity < size) {
        realloc(size);
    }
    std::uninitialized_fill_n(m_buffer, size, value);

    m_size = size;
    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::assign(std::initializer_list<T> list) {
    clear();
    auto size = list.size();

    if (size > m_capacity) {
        realloc(size);
    }

    m_size = size;
    std::uninitialized_copy(list.begin(), list.end(), m_buffer);

    return *this;
}

template <class T>
constexpr bool Vector<T>::operator==(const Vector<T> &v) const {
    return std::equal(begin(), end(), v.begin(), v.end());
}

template <class T>
constexpr bool Vector<T>::operator!=(const Vector<T> &v) const {
    return !operator==(v);
}

template <class T>
constexpr bool Vector<T>::operator<(const Vector<T> &v) const {
    return std::lexicographical_compare(begin(), end(), v.begin(), v.end());
}

template <class T>
constexpr bool Vector<T>::operator>(const Vector<T> &v) const {
    return !operator<(v) && !operator==(v);
    // return std::lexicographical_compare(begin(), end(), v.begin(), v.end(), std::greater<>{});
}

template <class T>
constexpr bool Vector<T>::operator<=(const Vector<T> &v) const {
    return !operator>(v);
}

template <class T>
constexpr bool Vector<T>::operator>=(const Vector<T> &v) const {
    return !operator<(v);
}

template <class T>
constexpr typename Vector<T>::iterator Vector<T>::begin() noexcept {
    return m_buffer;
}

template <class T>
constexpr typename Vector<T>::const_iterator Vector<T>::begin() const noexcept {
    return m_buffer;
}

template <class T>
constexpr typename Vector<T>::iterator Vector<T>::end() noexcept {
    return m_buffer + m_size;
}

template <class T>
constexpr typename Vector<T>::const_iterator Vector<T>::end() const noexcept {
    return m_buffer + m_size;
}

template <class T>
constexpr typename Vector<T>::reverse_iterator Vector<T>::rbegin() noexcept {
    return std::make_reverse_iterator(end());
}

template <class T>
constexpr typename Vector<T>::const_reverse_iterator Vector<T>::rbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <class T>
constexpr typename Vector<T>::reverse_iterator Vector<T>::rend() noexcept {
    return std::make_reverse_iterator(begin());
}

template <class T>
constexpr typename Vector<T>::const_reverse_iterator Vector<T>::rend() const noexcept {
    return std::make_reverse_iterator(begin());
}

template <class T>
constexpr typename Vector<T>::const_iterator Vector<T>::cbegin() const noexcept {
    return m_buffer;
}

template <class T>
constexpr typename Vector<T>::const_reverse_iterator Vector<T>::crbegin() const noexcept {
    return std::make_reverse_iterator(end());
}

template <class T>
constexpr typename Vector<T>::const_iterator Vector<T>::cend() const noexcept {
    return m_buffer + m_size;
}

template <class T>
constexpr typename Vector<T>::const_reverse_iterator Vector<T>::crend() const noexcept {
    return std::make_reverse_iterator(begin());
}

template <class T>
constexpr bool Vector<T>::empty() const noexcept {
    return m_size == 0;
}

template <class T>
constexpr typename Vector<T>::size_type Vector<T>::size() const noexcept {
    return m_size;
}

template <class T>
constexpr typename Vector<T>::size_type Vector<T>::max_size() const noexcept {
    return std::numeric_limits<size_type>::max() / sizeof(T);
}

template <class T>
constexpr typename Vector<T>::size_type Vector<T>::capacity() const noexcept {
    return m_capacity;
}

template <class T>
constexpr Vector<T> &Vector<T>::resize(size_type size) {
    if (size > m_size) {
        reserve(size);
        std::uninitialized_fill_n(end(), size - m_size, T{});
        m_size = size;
    } else if (size < m_size) {
        erase(end() - size, end());
    }
    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::resize(size_type size, const_reference value) {
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
constexpr Vector<T> &Vector<T>::shrink_to_fit() {
    auto buffer = alloc(m_size);

    std::uninitialized_move_n(m_buffer, m_size, buffer);
    std::destroy_n(m_buffer, m_size);
    std::free(m_buffer);

    m_buffer = buffer;
    m_capacity = m_size;

    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::reserve(size_type size) {
    if (size > max_size()) {
        throw std::length_error("al::Vector: length error");
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
constexpr typename Vector<T>::reference Vector<T>::operator[](size_type index) {
    return m_buffer[index];
}

template <class T>
constexpr typename Vector<T>::const_reference Vector<T>::operator[](size_type index) const {
    return m_buffer[index];
}

template <class T>
constexpr typename Vector<T>::reference Vector<T>::at(size_type index) {
    if (index < m_size) {
        return operator[](index);
    }
    throw std::out_of_range("al::Vector: out of range");
}

template <class T>
constexpr typename Vector<T>::const_reference Vector<T>::at(size_type index) const {
    if (index < m_size) {
        return operator[](index);
    }
    throw std::out_of_range("al::Vector: out of range");
}

template <class T>
constexpr typename Vector<T>::pointer Vector<T>::data() noexcept {
    return m_buffer;
}

template <class T>
constexpr typename Vector<T>::const_pointer Vector<T>::data() const noexcept {
    return m_buffer;
}

template <class T>
constexpr typename Vector<T>::reference Vector<T>::front() {
    return *begin();
}

template <class T>
constexpr typename Vector<T>::const_reference Vector<T>::front() const {
    return *begin();
}

template <class T>
constexpr typename Vector<T>::reference Vector<T>::back() {
    return *(end() - 1);
}

template <class T>
constexpr typename Vector<T>::const_reference Vector<T>::back() const {
    return *(end() - 1);
}

template <class T>
template <class... Args>
constexpr typename Vector<T>::reference Vector<T>::emplace_back(Args &&...args) {
    if (m_size == m_capacity) {
        realloc();
    }
    ::new (m_buffer + m_size) T(std::forward<Args>(args)...);
    ++m_size;

    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::push_back(const_reference value) {
    if (m_size == m_capacity) {
        realloc();
    }
    ::new (m_buffer + m_size) T(value);
    ++m_size;

    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::push_back(rvalue_reference value) {
    if (m_size == m_capacity) {
        realloc();
    }
    ::new (m_buffer + m_size) T(std::move(value));
    ++m_size;

    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::pop_back() {
    std::destroy_at(m_buffer + --m_size);
    return *this;
}

template <class T>
template <class... Args>
constexpr typename Vector<T>::iterator Vector<T>::emplace(const_iterator pos, Args &&...args) {
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
constexpr typename Vector<T>::iterator Vector<T>::insert(const_iterator pos, It first, It last) {
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

template <class T>
constexpr typename Vector<T>::iterator Vector<T>::insert(const_iterator pos, const_reference value) {
    auto diff = std::distance(m_buffer, pos);

    if (m_size == m_capacity) {
        realloc();
        pos = m_buffer + diff;
    }
    std::move_backward(pos, end(), end() + 1);
    ::new (pos) T(value);

    ++m_size;
    return m_buffer + diff;
}

template <class T>
constexpr typename Vector<T>::iterator Vector<T>::insert(const_iterator pos, rvalue_reference value) {
    auto diff = std::distance(m_buffer, pos);

    if (m_size == m_capacity) {
        realloc();
        pos = m_buffer + diff;
    }
    std::move_backward(pos, end(), end() + 1);
    ::new (pos) T(std::move<T>(value));

    ++m_size;
    return m_buffer + diff;
}

template <class T>
constexpr typename Vector<T>::iterator Vector<T>::insert(const_iterator pos, size_type size, const_reference value) {
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
constexpr typename Vector<T>::iterator Vector<T>::insert(const_iterator pos, std::initializer_list<T> list) {
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
constexpr typename Vector<T>::iterator Vector<T>::erase(const_iterator pos) {
    std::destroy_at(pos);
    std::move(pos + 1, end(), pos);

    --m_size;
    return pos;
}

template <class T>
constexpr typename Vector<T>::iterator Vector<T>::erase(const_iterator first, const_iterator last) {
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
constexpr Vector<T> &Vector<T>::swap(Vector<T> &v) noexcept {
    std::swab(this, &v, sizeof(Vector<T>));
    return *this;
}

template <class T>
constexpr Vector<T> &Vector<T>::clear() noexcept {
    std::destroy_n(m_buffer, m_size);
    m_size = 0;
    return *this;
}

template <class T>
constexpr typename Vector<T>::pointer Vector<T>::alloc(size_type size) noexcept {
    auto res = static_cast<pointer>(std::malloc(size * sizeof(T)));

    if (res == nullptr) {
        throw std::bad_alloc();
    }
    return res;
}

template <class T>
constexpr void Vector<T>::realloc(size_type size) noexcept {
    auto capacity = std::max(m_capacity ? m_capacity * 2 : 1, size);
    auto buffer = alloc(capacity);

    std::uninitialized_move_n(m_buffer, m_size, buffer);
    std::destroy_n(m_buffer, m_size);
    std::free(m_buffer);

    m_buffer = buffer;
    m_capacity = capacity;
}

} // namespace al
