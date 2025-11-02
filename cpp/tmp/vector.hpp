#pragma once

#include <iterator> // needed for iterator traits
#include <stdio.h>  // todo: try this with cstio

namespace dev_std {

template <typename T> class vector {
public:
  // defualt ctor
  vector();
  // copy ctors
  vector(const vector &);
  vector(vector &&) noexcept;

  vector(size_t,
         const T &); // should not be explicit (should be allowed to be rvalues)
  explicit vector(size_t); // prevent dev_std::vector<char> v = 5;

  // destructor
  ~vector();

  // initializer list

  // insert
  void push_back(const T &);
  void push_back(T &&);

  template <typename... Args> void emplace_back(Args &&...);

  // delete
  void pop_back();

  // quick stats
  size_t size() const noexcept;
  bool empty() const noexcept;

  T &front();
  const T &front() const;
  T &back();
  const T &back() const;

  // todo iterator class

  void reserve(size_t);
  void resize(size_t);

  void clear() noexcept;

  // should this be caps or does it not mappter
  class iterator {
  public:
    // traits
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;
    using iterator_category =
        std::random_access_iterator_tag; // iterator categories

    iterator operator++(int); // postfix
    iterator &operator++();   // prefix
    iterator operator+(int);
    iterator operator-(int);
    iterator &operator+=(int);
    iterator &operator-=(int);
    iterator &operator=(const iterator &other);
    iterator &operator=(iterator &&other) noexcept;
    bool operator==(const iterator &) const noexcept;
    bool operator!=(const iterator &) const noexcept;
    explicit operator bool() const noexcept;
    T &operator*() const noexcept;
    T *operator->() const noexcept;

  private:
    T *ptr_;
  };

  friend class iterator; // allow data member access

  // const_iterator begin() const; -> add with const_iterator begin() const;
  iterator begin();
  iterator end();
  // const_iterator end() const;

  // operators
  // assignment
  vector &operator=(const vector &);
  vector &operator=(vector &&) noexcept;
  bool operator==(const vector &) const noexcept;
  bool operator!=(const vector &) const noexcept;

  // indexing operator
  T &operator[](size_t);
  const T &operator[](size_t) const;

  // iterator functions
  iterator insert(const iterator &, const iterator &, const iterator &);
  iterator insert(const iterator &, const T &);
  iterator insert(const iterator &, T &&);
  iterator erase(const iterator &);

private:
  T *data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

  // double the size
  void resize_(size_t);
  void copy_(const vector &);
  void move_(vector &&) noexcept;
  void destroy_() noexcept;
};

/* todo:
    half it when the size drops below a certain amount
*/

} // namespace dev_std

/*


can do something like this to access anything that's passed in
friend bool operator==(vector_iter a, vector_iter b){ return a.p_ == b.p_; }




TODO check: initializer list
Traits in iterators


friend class iterator; lets iterator access vector’s privates; if vector needs
ptr_, either make iterator(T*) public or friend class vector<T>; inside
iterator.


Example of traits with iterators:
auto dist = std::distance(it1, it2);
auto next_it = std::next(it, 2)
std::advance(it, 3);
std::sort(v.begin(), v.end());

Getting the info
template<typename Iter>
void print_info() {
    using traits = std::iterator_traits<Iter>;
    using value_type = typename traits::value_type;  // Type iterator points to
    using diff_type = typename traits::difference_type;  // Usually ptrdiff_t
}



template<typename Iter>
typename std::iterator_traits<Iter>::value_type sum(Iter first, Iter last) {
    typename std::iterator_traits<Iter>::value_type result = 0;
    for (; first != last; ++first) {
        result += *first;
    }
    return result;
}


    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;





    Iterator categories (least to most flexible):

output_iterator_tag - Write-only, single pass forward (e.g.,
std::ostream_iterator) input_iterator_tag - Read-only, single pass forward
(e.g., std::istream_iterator) forward_iterator_tag - Read/write, multi-pass
forward only (e.g., std::forward_list) bidirectional_iterator_tag - Forward +
backward with -- (e.g., std::list, std::map) random_access_iterator_tag - Jump
anywhere with +, -, [] in O(1) (e.g., std::vector, std::deque)
contiguous_iterator_tag (C++20) - Random access + guaranteed contiguous memory
(e.g., std::vector, arrays)



other classes to add
const_iterator
reverse_iterator
const_reverse_iterator


pragmas
*/
