namespace dev_std {

/**
 * @brief Allows for the forwarding of references
 * without expensive copies. See Barriers for sample
 * use
 *
 */
template <typename T> class reference {
private:
  T *ptr;

public:
  explicit reference(T &obj) : ptr(&obj) {}
  operator T &() { return *ptr; } // conversion operator
  T &get() const { return *ptr; }
};

template <typename T> reference<T> ref(T &t) { return reference<T>(t); }

template <typename T> reference<const T> cref(const T &t) {
  return reference<const T>(t);
}
} // namespace dev_std