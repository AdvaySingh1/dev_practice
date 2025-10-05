#pragma once

#include <utility>
/*
// Simplified version of what's in <type_traits>
template<typename T>
struct remove_reference {
    typedef T type;  // 'type' is a type alias (like using type = T;)
};

template<typename T>
struct remove_reference<T&> {
    typedef T type;  // Specialization for lvalue references
};

template<typename T>
struct remove_reference<T&&> {
    typedef T type;  // Specialization for rvalue references
};

template<typename T>
typename remove_reference<T>::type&& move(T&& val) {
    return static_cast<remove_reference<T>::T&&>(val);
}
*/


namespace dev_std {
template<typename T1, typename T2>
class pair {
    public:
    T1 a; T2 b;
    pair() : a(), b() {}
    pair(const T1& a, const T2& b): a(a), b(b){}
    pair(T1&& a, T2&& b): a(std::move(a)), b(std::move(b)){}
    pair(const pair& p): a(p.a), b(p.b) {}
    pair(pair&& p): a(std::move(p.a)), b(std::move(p.b)) {}

    [[nodiscard]] T1& first() { return a; }
    [[ nodiscard ]] T2& second() { return b; }

    [[ nodiscard ]] const T1& first() const { return a; }
    [[ nodiscard ]] const T2& second() const { return b; }

    pair& operator=(const pair& p) { a = p.a; b = p.b; return *this; };
    pair& operator=(pair&& p) { a = std::move(p.a), b = std::move(p.b); return *this; };

    bool operator==(const pair& p) const { return a == p.a && b == p.b; };
    bool operator!=(const pair& p) const { return !(*this == p); };
    bool operator<(const pair& p) const { return a == p.a ? b < p.b : a < p.a; };
    bool operator>(const pair& p) const { return a == p.a ? b > p.b : a > p.a; };
    bool operator<=(const pair& p) const { return !(*this > p); };
    bool operator>=(const pair& p) const { return !(*this < p); };
    private:
};

template<typename T1, typename T2>
pair<T1, T2> make_pair(T1&& a, T2&& b) { return pair<T1, T2>(std::forward<T1>(a), std::forward<T2>(b)); } // in this case, it allows it to keep its forwarding reference
template<typename T1, typename T2>
void swap(pair<T1, T2>& p_a, pair<T1, T2>& p_b) { std::swap(p_a.a, p_b.a); std::swap(p_a.b, p_b.b); }

}