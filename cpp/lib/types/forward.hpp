#pragma once

namespace dev_std {
template <typename T>
/**
 * @brief universal reference forwarding
 * (whatever the type is, use that)
 * 
 * The return type is T&& because:
 * - If T is deduced as int&, T&& collapses to int& (lvalue ref)
 * - If T is deduced as int, T&& becomes int&& (rvalue ref)
 * This preserves the value category of the original argument
 * 
 * If the return type was T, and T was deducted as T,
 * then we would be making a copy of the argumnet.
 */
T&& forward(T&& v) {
    // return v; <- this would cause it to turn into an lvalue if it was an rvalue
    // Without the cast, we'd always return an lvalue reference
    return static_cast<T&&>(v);
}


/* Example uses:
 Example 1: Perfect forwarding in wrapper functions
template<typename T>
void wrapper(T&& arg) {
     Without forward: arg is always treated as lvalue
    process(dev_std::forward<T>(arg));  Preserves lvalue/rvalue-ness
}

 Example 2: Factory function
template<typename T, typename... Args>
T* create(Args&&... args) {
    return new T(dev_std::forward<Args>(args)...);
}

 Example 3: Understanding the behavior
void use_example() {
    std::string s = "hello";
    
    wrapper(s);               T deduced as string&, forwards as lvalue
    wrapper(std::string(""));  T deduced as string, forwards as rvalue
}
*/
};
