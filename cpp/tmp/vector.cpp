#include "vector.hpp"
#include <stdio.h> // todo: try this with cstio

namespace dev_std {

// default ctor
template <typename T> vector<T>::vector() = default;

// copy ctors
template <typename T> vector<T>::vector(const vector &) {}

template <typename T> vector<T>::vector(vector &&) noexcept {}

template <typename T> vector<T>::vector(size_t, const T &) {}

template <typename T> vector<T>::vector(size_t) {}

// destructor
template <typename T> vector<T>::~vector() {}

// initializer list

// insert

template <typename T> void vector<T>::push_back(const T &) {}
template <typename T> void vector<T>::push_back(T &&) {}

template <typename T>
template <typename... Args>
void vector<T>::emplace_back(Args &&...) {}

// delete
template <typename T> void vector<T>::pop_back() {}

// quick stats
template <typename T> size_t vector<T>::size() const noexcept {}
template <typename T> bool vector<T>::empty() const noexcept {}

template <typename T> T &vector<T>::front() {}
template <typename T> const T &vector<T>::front() const {}
template <typename T> T &vector<T>::back() {}
template <typename T> const T &vector<T>::back() const {}

// todo iterator class
template <typename T> void vector<T>::reserve(size_t) {}
template <typename T> void vector<T>::resize(size_t) {}
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
template <typename T> vector<T> &vector<T>::operator=(const vector &) {}
template <typename T> vector<T> &vector<T>::operator=(vector &&) noexcept {}
template <typename T>
bool vector<T>::operator==(const vector &) const noexcept {}
template <typename T>
bool vector<T>::operator!=(const vector &) const noexcept {}

// indexing operator
template <typename T> T &vector<T>::operator[](size_t) {}
template <typename T> const T &vector<T>::operator[](size_t) const {}

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

} // namespace dev_std