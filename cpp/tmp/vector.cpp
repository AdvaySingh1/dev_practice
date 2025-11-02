#include "vector.hpp"
#include <memory>
#include <stdexcept> // std::out_of_range and has std::exception
#include <stdio.h>   // todo: try this with cstio

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

template <typename T> vector<T>::vector(size_t, const T &) {}

template <typename T> vector<T>::vector(size_t) {}

// destructor
template <typename T> vector<T>::~vector() { destroy_(); }

// initializer list

// insert

template <typename T> void vector<T>::push_back(const T &val) {
  if (size_ * 2 >= capacity_) {
    resize_(capacity_ * 2);
  }
  ::new (data_ + size_) T(val);
}
template <typename T> void vector<T>::push_back(T &&val) {
  if (size_ * 2 >= capacity_) {
    resize_(capacity_ * 2);
  }
  ::new (data_ + size_)
      T(std::move(val)); // need placement new because of uninit data
}

template <typename T>
template <typename... Args>
void vector<T>::emplace_back(Args &&...) {}

// delete
template <typename T> void vector<T>::pop_back() {
  if (empty()) {
    throw std::logic_error("Popping with 0 elements");
  }
  size_--;
  if (size_ * 2 < capacity_ / 2) { // 1 / 4 usage only
    resize_(capacity_ / 2);
  }
}

// quick stats
template <typename T> size_t vector<T>::size() const noexcept {}
template <typename T> bool vector<T>::empty() const noexcept {}

template <typename T> T &vector<T>::front() {}
template <typename T> const T &vector<T>::front() const {}
template <typename T> T &vector<T>::back() {}
template <typename T> const T &vector<T>::back() const {}

// todo iterator class
template <typename T> void vector<T>::reserve(size_t) {}
template <typename T> void vector<T>::resize(size_t size) {}
template <typename T> void vector<T>::clear() noexcept {}

template <typename T>
typename vector<T>::iterator vector<T>::iterator::operator++(int) {} // postfix
template <typename T>
typename vector<T>::iterator &vector<T>::iterator::operator++() {} // prefix
template <typename T>
typename vector<T>::iterator vector<T>::iterator::operator+(int) {}
template <typename T>
typename vector<T>::iterator vector<T>::iterator::operator-(int) {}
template <typename T>
typename vector<T>::iterator &vector<T>::iterator::operator+=(int) {}
template <typename T>
typename vector<T>::iterator &vector<T>::iterator::operator-=(int) {}
template <typename T>
typename vector<T>::iterator &
vector<T>::iterator::operator=(const iterator &other) {}
template <typename T>
typename vector<T>::iterator &
vector<T>::iterator::operator=(iterator &&other) noexcept {}
template <typename T>
bool vector<T>::iterator::operator==(const iterator &) const noexcept {}
template <typename T>
bool vector<T>::iterator::operator!=(const iterator &) const noexcept {}
template <typename T> vector<T>::iterator::operator bool() const noexcept {}
template <typename T> T &vector<T>::iterator::operator*() const noexcept {}
template <typename T> T *vector<T>::iterator::operator->() const noexcept {}

template <typename T> typename vector<T>::iterator begin() {}
template <typename T> typename vector<T>::iterator end() {}

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