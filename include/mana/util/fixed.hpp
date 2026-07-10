#pragma once

#include <cstddef>
#include <cstring>
#include <algorithm>
#include <string_view>
#include <memory>
#include <initializer_list>
#include <stdexcept>

namespace mana::util {

// Fixed-size string (supports NTTP in C++20)
template<size_t N>
class fixed_string {
public:
    char data_[N]{};
    size_t size_ = 0;

    constexpr fixed_string() = default;

    constexpr fixed_string(const char (&str)[N]) {
        for (size_t i = 0; i < N && str[i] != '\0'; ++i) {
            data_[i] = str[i];
            ++size_;
        }
    }

    // Accept string literals of different size (smaller or larger)
    template<size_t M>
    constexpr fixed_string(const char (&str)[M]) {
        constexpr size_t count = (M < N) ? M : N;
        for (size_t i = 0; i < count && str[i] != '\0'; ++i) {
            data_[i] = str[i];
            ++size_;
        }
    }

    constexpr size_t size() const noexcept { return size_; }
    constexpr const char* c_str() const noexcept { return data_; }
    constexpr std::string_view view() const noexcept { return {data_, size_}; }

    // Cross-size equality comparison (also handles same-size via M==N)
    template<size_t M>
    constexpr bool operator==(const fixed_string<M>& other) const {
        if (size_ != other.size_) return false;
        for (size_t i = 0; i < size_; ++i) {
            if (data_[i] != other.data_[i]) return false;
        }
        return true;
    }

    constexpr auto operator<=>(const fixed_string& other) const {
        for (size_t i = 0; i < std::min(size_, other.size_); ++i) {
            if (data_[i] != other.data_[i]) {
                return data_[i] <=> other.data_[i];
            }
        }
        return size_ <=> other.size_;
    }

    template<size_t M>
    constexpr fixed_string<N + M - 1> operator+(const fixed_string<M>& other) const {
        fixed_string<N + M - 1> result;
        for (size_t i = 0; i < size_; ++i) {
            result.data_[result.size_++] = data_[i];
        }
        for (size_t i = 0; i < other.size_; ++i) {
            result.data_[result.size_++] = other.data_[i];
        }
        return result;
    }
};

// CTAD deduction guide
template<size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

// Fixed-capacity dynamic array (constexpr-friendly)
template<typename T, size_t N>
class fixed_vector {
public:
    T data_[N]{};
    size_t size_ = 0;

    constexpr fixed_vector() = default;

    constexpr fixed_vector(std::initializer_list<T> init) {
        for (const auto& item : init) {
            push_back(item);
        }
    }

    constexpr void push_back(const T& value) {
        if (size_ >= N)
#ifdef __cpp_constexpr_exceptions
            throw std::overflow_error("fixed_vector: capacity exceeded");
#else
            __assume(false);
#endif
        data_[size_++] = value;
    }

    constexpr void push_back(T&& value) {
        if (size_ >= N)
#ifdef __cpp_constexpr_exceptions
            throw std::overflow_error("fixed_vector: capacity exceeded");
#else
            __assume(false);
#endif
        data_[size_++] = std::move(value);
    }

    constexpr void pop_back() {
        if (size_ > 0) --size_;
    }

    constexpr void clear() noexcept { size_ = 0; }

    constexpr T& operator[](size_t i) { return data_[i]; }
    constexpr const T& operator[](size_t i) const { return data_[i]; }

    constexpr size_t size() const noexcept { return size_; }
    constexpr size_t capacity() const noexcept { return N; }
    constexpr bool empty() const noexcept { return size_ == 0; }
    constexpr bool full() const noexcept { return size_ == N; }

    constexpr T* begin() noexcept { return data_; }
    constexpr T* end() noexcept { return data_ + size_; }
    constexpr const T* begin() const noexcept { return data_; }
    constexpr const T* end() const noexcept { return data_ + size_; }
};

// Small buffer optimization vector
template<typename T, size_t N>
class small_vector {
    alignas(T) char stack_buf_[N * sizeof(T)]{};
    T* data_ = reinterpret_cast<T*>(stack_buf_);
    size_t size_ = 0;
    size_t capacity_ = N;

    void grow() {
        size_t new_cap = capacity_ * 2;
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        if (data_ != reinterpret_cast<T*>(stack_buf_)) {
            ::operator delete(data_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }

public:
    constexpr small_vector() = default;

    small_vector(const small_vector& other) {
        reserve(other.size_);
        for (size_t i = 0; i < other.size_; ++i) {
            push_back(other.data_[i]);
        }
    }

    small_vector(small_vector&& other) noexcept {
        if (other.data_ == reinterpret_cast<T*>(other.stack_buf_)) {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(std::move(other.data_[i]));
            }
        } else {
            data_ = other.data_;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data_ = reinterpret_cast<T*>(other.stack_buf_);
            other.size_ = 0;
            other.capacity_ = N;
        }
    }

    ~small_vector() {
        clear();
        if (data_ != reinterpret_cast<T*>(stack_buf_)) {
            ::operator delete(data_);
        }
    }

    void reserve(size_t new_cap) {
        if (new_cap <= capacity_) return;
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        if (data_ != reinterpret_cast<T*>(stack_buf_)) {
            ::operator delete(data_);
        }
        data_ = new_data;
        capacity_ = new_cap;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) grow();
        new (data_ + size_) T(value);
        ++size_;
    }

    void push_back(T&& value) {
        if (size_ >= capacity_) grow();
        new (data_ + size_) T(std::move(value));
        ++size_;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
            data_[size_].~T();
        }
    }

    void clear() noexcept {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    T& operator[](size_t i) { return data_[i]; }
    const T& operator[](size_t i) const { return data_[i]; }

    size_t size() const noexcept { return size_; }
    size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }
    bool full() const noexcept { return size_ == capacity_; }

    T* begin() noexcept { return data_; }
    T* end() noexcept { return data_ + size_; }
    const T* begin() const noexcept { return data_; }
    const T* end() const noexcept { return data_ + size_; }
};

} // namespace mana::util
