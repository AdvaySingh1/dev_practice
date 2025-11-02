#include "vector.hpp"
#include <memory>
#include <stdexcept> // std::out_of_range and has std::exception
#include <stdio.h>   // todo: try this with cstio
#include <utility>

namespace dev_std {

// default ctor
template <typename T> vector<T>::vector() : size_(0), capacity_(5) {
  data_ = static_cast<T *>(::operator new(sizeof(T) * capacity_));
};

// copy ctors
template <typename T> vector<T>::vector(const vector &other) { copy_(other); }

template <typename T> vector<T>::vector(vector &&other) noexcept {
  move_(std::move(other));
}

template <typename T>
vector<T>::vector(size_t, const T &val) : size_(size), capacity_(size_ * 2) {
  data_ = static_cast<T *>(::operator new(sizeof(T) * capacity_));
  for (int i = 0; i < size_; ++i) {
    ::new (data_ + i) T(val);
  }
}

template <typename T>
vector<T>::vector(size_t size_) : size_(size), capacity_(size_ * 2) {
  data_ = static_cast<T *>(::operator new(sizeof(T) * capacity_));
  for (int i = 0; i < size_; ++i) {
    ::new (data_ + i) T();
  }
}

// destructor
template <typename T> vector<T>::~vector() { destroy_(); }

// initializer list

// insert

template <typename T> void vector<T>::push_back(const T &val) {
  if (size_ * 2 >= capacity_) {
    resize_(capacity_ * 2);
  }
  ::new (data_ + size_++) T(val);
}
template <typename T> void vector<T>::push_back(T &&val) {
  if (size_ * 2 >= capacity_) {
    resize_(capacity_ * 2);
  }
  ::new (data_ + size_++)
      T(std::move(val)); // need placement new because of uninit data
}

template <typename T>
template <typename... Args>
void vector<T>::emplace_back(Args &&...args) {
  if (size_ * 2 >= capacity_) {
    resize_(capacity_ * 2);
  }
  ::new (data_ + size_++) T(std::forward<Args>(args)...);
}

// delete
template <typename T> void vector<T>::pop_back() {
  if (empty()) {
    throw std::logic_error("Popping with 0 elements");
  }
  size_--;
  delete data_[size_];
  if (size_ * 2 < capacity_ / 2) { // 1 / 4 usage only
    resize_(capacity_ / 2);
  }
}

// quick stats
template <typename T> size_t vector<T>::size() const noexcept { return size_; }
template <typename T> bool vector<T>::empty() const noexcept {
  return size_ == 0;
}

template <typename T> T &vector<T>::front() {
  if (empty()) {
    throw std::logic_error(
        "dev_std::vector::front instantiated with 0 elements");
  }
  return data_[0];
}
template <typename T> const T &vector<T>::front() const {
  if (empty()) {
    throw std::logic_error(
        "dev_std::vector::front instantiated with 0 elements");
  }
  return data_[0];
}
template <typename T> T &vector<T>::back() {
  if (empty()) {
    throw std::logic_error(
        "dev_std::vector::back instantiated with 0 elements");
  }
  return data_[size_ - 1];
}
template <typename T> const T &vector<T>::back() const {
  if (empty()) {
    throw std::logic_error(
        "dev_std::vector::back instantiated with 0 elements");
  }
  return data_[size_ - 1];
}

// todo iterator class
template <typename T> void vector<T>::reserve(size_t capacity) {
  resize_(capacity);
}
template <typename T> void vector<T>::resize(size_t size) {
  if (size < size_) {
    throw std::logic_error(
        "dev_std::vector::resize() instantiated with size smaller \
                      than the current size ");
  }

  if (size >= capacity_ * 2) {
    resize_(size * 2);
  }
  for (; size_ < size; ++size_) {
    ::new (data_ + size_) T();
  }
}
template <typename T> void vector<T>::clear() noexcept {
  destroy_();
  resize_(5);
}

template <typename T>
typename vector<T>::iterator vector<T>::iterator::operator++(int) noexcept {
  return iterator(ptr_++);
} // postfix
template <typename T>
typename vector<T>::iterator &vector<T>::iterator::operator++() noexcept {
  return iterator(++ptr_);
} // prefix
template <typename T>
typename vector<T>::iterator vector<T>::iterator::operator--(int) noexcept {
  return iterator(ptr_--);
} // postfix
template <typename T>
typename vector<T>::iterator &vector<T>::iterator::operator--() noexcept {
  return iterator(--ptr_);
} // prefix
template <typename T>
typename vector<T>::iterator
vector<T>::iterator::operator+(int val) const noexcept {
  return iterator(ptr_ + val);
}
template <typename T>
typename vector<T>::iterator
vector<T>::iterator::operator-(int val) const noexcept {
  return iterator(ptr_ - val);
}
template <typename T>
typename vector<T>::iterator &
vector<T>::iterator::operator+=(int val) noexcept {
  ptr_ += val;
  return *this;
}
template <typename T>
typename vector<T>::iterator &
vector<T>::iterator::operator-=(int val) noexcept {
  ptr_ -= val;
  return *this;
}
template <typename T>
bool vector<T>::iterator::operator==(const iterator &other) const noexcept {
  return ptr_ == other.ptr_;
}
template <typename T>
bool vector<T>::iterator::operator!=(const iterator &other) const noexcept {
  return ptr_ != other.ptr_;
}
template <typename T> vector<T>::iterator::operator bool() const noexcept {
  return ptr_ != nullptr;
}
template <typename T> T &vector<T>::iterator::operator*() const noexcept {
  return *ptr_;
}
template <typename T> T *vector<T>::iterator::operator->() const noexcept {
  return ptr_;
}

template <typename T>
vector<T>::iterator::iterator(T *ptr) noexcept : ptr_(ptr) {}

template <typename T> typename vector<T>::iterator vector<T>::begin() {
  return iterator(data_);
}
template <typename T> typename vector<T>::iterator vector<T>::end() {
  return iterator(data_ + size_ + 1);
}

// operators
// assignment
template <typename T> vector<T> &vector<T>::operator=(const vector &other) {
  _copy(other);
}
template <typename T> vector<T> &vector<T>::operator=(vector &&other) noexcept {
  _move(std::move(other));
}
template <typename T>
bool vector<T>::operator==(const vector &other) const noexcept {
  return this == &other;
}
template <typename T>
bool vector<T>::operator!=(const vector &other) const noexcept {
  return this != &other;
}

// indexing operator
template <typename T> T &vector<T>::operator[](size_t i) {
  if (i >= size_) {
    throw std::out_of_range("Indexing out of range");
  }
  return *(data_ + i);
}
template <typename T> const T &vector<T>::operator[](size_t i) const {
  if (i >= size_) {
    throw std::out_of_range("Indexing out of range");
  }
  return *(data_ + i);
}

// iterator functions (note auto can be used here)
template <typename T>
typename vector<T>::iterator
vector<T>::insert(const iterator &, const iterator &, const iterator &) {}
template <typename T>
typename vector<T>::iterator vector<T>::insert(const iterator &, const T &) {}
template <typename T>
typename vector<T>::iterator vector<T>::insert(const iterator &, T &&) {}
template <typename T>
typename vector<T>::iterator vector<T>::erase(const iterator &) {}

// helper functions
template <typename T> void vector<T>::resize_(size_t new_capacity) {
  if (new_capacity < size_) {
    throw std::logic_error(
        "Vector attempted resize with capacity less than size");
  }
  // move this to tmp
  vector tmp(std::move(*this));
  capacity_ = new_capacity;
  size_ = tmp.size_;
  data_ = static_cast<T *>(::operator new(sizeof(T) * capacity_));
  std::uninitialized_move(tmp.begin(), tmp.end(), begin());
  // still need to call the destructor despite this move
}

template <typename T> void vector<T>::copy_(const vector &other) {
  destroy_();
  size_ = other.size_;
  capacity_ = other.capacity_;
  data_ = static_cast<T *>(::operator new( // static_cast for compiler check
      sizeof(T) *
      capacity_)); // ::operator new for malloc like function with no ctor

  // does ::new (data_+i) T(other[i]);
  std::uninitialized_copy(other.begin(), other.end(), begin());

  /*
  The following calls a default ctor on all of them
  followed by a copy ctor (done in the new)
  data_ = new T[capacity_];
  for (size_t i = 0; i < other.size; ++i) {
    data_[i](other.data_[i]); // construct for larger objects
  }
    */
}
template <typename T> void vector<T>::move_(vector &&other) noexcept {
  destroy_();
  size_ = other.size_;
  capacity_ = other.capacity_;
  data_ = other.data_;

  other.size_ = 0;
  other.capacity_ = 0;
  other.data_ = nullptr;
}

template <typename T> void vector<T>::destroy_() noexcept {
  // not calling destructor on un-init data
  std::destroy(data_, data_ + size_);
  ::operator delete(data_);
  data_ = nullptr;
  size_ = 0;
  capacity_ = 0;
}

} // namespace dev_std