#pragma once
#include <cstddef>
#include "types.hpp"

#ifdef DEBUG
#include <iostream>
#endif


namespace dev_std {

    /**
     * @todo when the ctor is called, the universal forward reference
     * (&&) is called rather than the rvalue ctor due to templating.
     * Hence, in order to explitly call the lvalue ctor, the value
     * passed in needs to be a const lvalue.
     * 
     * 
     * 
     * 
     */

template <typename R, typename... Args>
class function;

// synopsys
template <typename R, typename... Args>
class function<R(Args...)> {
    private: // QUES: should these be in copy? Would there ever be a class which wants to inherit from this?
    // function holder
    void* fn_holder_ = nullptr;
    // function invoker
    R (*invoker_)(void*, Args...) = nullptr;
    // function initializer
    void* (*fn_alloc_)(void*) = nullptr;
    // function deleter
    void (*deleter_)(void*) = nullptr;

    public:
    // default ctor, init to nullptr
    function() = default;
    // copy ctor. Copy the function and alloc onto heap
    function(const function&);
    // move ctor. Move the function and alloc onto heap
    function(function&&) noexcept;
    // copy function ctor. Copy the function and alloc onto heap
    template <typename FN_T>
    function(const FN_T&);
    // move function ctor. Move the function and alloc onto heap
    template <typename FN_T>
    function(FN_T&&) noexcept;
    // assignment operator
    function& operator=(const function&);
    function& operator=(function&&) noexcept;

    // capacity operator
    [[nodiscard]] explicit operator bool() const noexcept;

    // comparison operator
    // [[nodiscard]] bool operator==(const function&) const noexcept;

    // invoker
    R operator()(Args...) const;

    // destructor
    ~function();

    private:
    // helper functions

    template <typename FN_T>
    void setHelperCallbacks();


};

// swap
template <typename R, typename... Args>
void swap(function<R(Args...)>&, function<R(Args...)>&) noexcept;

// nullptr comparisons
template <typename R, typename... Args>
[[nodiscard]] bool operator==(function<R(Args...)>, nullptr_t);

template <typename R, typename... Args>
[[nodiscard]] bool operator==(nullptr_t, function<R(Args...)>);

template <typename R, typename... Args>
[[nodiscard]] bool operator!=(function<R(Args...)>, nullptr_t);

template <typename R, typename... Args>
[[nodiscard]] bool operator!=(nullptr_t, function<R(Args...)>);




/*
Implementations can't be in cpp files since you can't link against templated functions
as they are not compiled at compile time, rather, at object init.
Other files which only include the header file won't be able to see
the templated types.
*/

template <typename R, typename... Args>
function<R(Args...)>::function(const function& F) {
    #ifdef DEBUG 
        std::cout << "Calling copy ctor" << std::endl;
    #endif
    *this = F;
}
// function F is non-unversal reference passing -> known rvalue
template <typename R, typename... Args>
function<R(Args...)>::function(function&& F) noexcept 
    : fn_holder_(F.fn_holder_)
    , invoker_(F.invoker_)
    , fn_alloc_(F.fn_alloc_)
    , deleter_(F.deleter_)
{
    #ifdef DEBUG 
        std::cout << "Calling move ctor" << std::endl;
    #endif
    
    // Nullify source to prevent double-free
    F.fn_holder_ = nullptr;
    F.invoker_ = nullptr;
    F.fn_alloc_ = nullptr;
    F.deleter_ = nullptr;
}

template <typename R, typename... Args>
function<R(Args...)>& function<R(Args...)>::operator=(const function& F) {
    #ifdef DEBUG 
        std::cout << "Calling copy assignment operator" << std::endl;
    #endif
    if (*this) deleter_(fn_holder_);
    // deep copy the actual function
    fn_holder_ = F.fn_alloc_(reinterpret_cast<void*>(F.fn_holder_));
    // these are stateless and global
    invoker_ = F.invoker_;
    fn_alloc_ = F.fn_alloc_; 
    deleter_ = F.deleter_;
    return *this;
}

template <typename R, typename... Args>
function<R(Args...)>& function<R(Args...)>::operator=(function&& F) noexcept {
    #ifdef DEBUG 
        std::cout << "Calling move assignment operator" << std::endl;
    #endif
    if (*this) deleter_(fn_holder_);
    // move heap ptr
    fn_holder_ = F.fn_holder_;
    invoker_ = F.invoker_;
    fn_alloc_ = F.fn_alloc_;
    deleter_ = F.deleter_;

    // avoid double frees 
    F.fn_holder_ = nullptr;
    F.invoker_ = nullptr;
    F.fn_alloc_ = nullptr;
    F.deleter_ = nullptr;
    return *this;
}
/*
    The following can be problamatic in certain cases when you copy
    member variables from other functions. For example, consider
    std::unique_pointers. This implicitly forwards ownership.
*/
template <typename R, typename... Args>
template <typename FN_T>
function<R(Args...)>::function(const FN_T& F) {
    // copy the other function into the pointer
    fn_holder_ = reinterpret_cast<void*>(new FN_T((F))); // ensure lvalue

    // set the invoker and deleter based on the type
    setHelperCallbacks<FN_T>();

    #ifdef DEBUG
    std::cout << "callind copy function_holder inference ctor" << std::endl;
    #endif

}

template <typename R, typename... Args>
template <typename FN_T>
function<R(Args...)>::function(FN_T&& F) noexcept {
    // copy the function object onto the heap
    fn_holder_ = reinterpret_cast<void*>(new FN_T(dev_std::move(F))); // ensure rvalue

    // set the invoker and deleter based on the type
    setHelperCallbacks<FN_T>();

    #ifdef DEBUG
    std::cout << "callind mvoe function_holder inference ctor" << std::endl;
    #endif
}

template <typename R, typename... Args>
[[nodiscard]] function<R(Args...)>::operator bool() const noexcept{
    return fn_holder_ != nullptr;
}

template <typename R, typename... Args>
R function<R(Args...)>::operator()(Args... args) const {
    if (!*this) {
        // TODO: throw and exception
        #ifdef DEBUG
        std::cerr << "Invoked empty functional class instance!";
        #endif
        return R();
    }
    return invoker_(fn_holder_, args...);
}

template <typename R, typename... Args>
function<R(Args...)>::~function() {
    if (*this) { // check if fn_holder_ is nullptr
        deleter_(fn_holder_);
    }
}

// template <typename R, typename... Args>
// void function<R(Args...)>::swap(function& f) {
//     function tmp(dev_std::move(*this));
//     *this = std::move(f);
//     f = std::move(tmp);
// }


template <typename R, typename... Args>
[[nodiscard]] bool operator==(function<R(Args...)> f, nullptr_t){
    return static_cast<bool>(f);
}

template <typename R, typename... Args>
[[nodiscard]] bool operator==(nullptr_t, function<R(Args...)> f){
    return static_cast<bool>(f);
}

template <typename R, typename... Args>
[[nodiscard]] bool operator!=(function<R(Args...)> f, nullptr_t){
    return static_cast<bool>(f);
}

template <typename R, typename... Args>
[[nodiscard]] bool operator!=(nullptr_t, function<R(Args...)> f){
    return static_cast<bool>(f);
}

// template <typename F>
// void swap(F& f1, F& f2) {
//     auto tmp(std::move(f1));
//     f1 = std::move(f2);
//     f2 = std::move(tmp);
// }
template <typename R, typename... Args>
void swap(function<R(Args...)>& f1, function<R(Args...)>& f2) noexcept {
    function<R(Args...)> tmp(std::move(f1));
    f1 = std::move(f2);
    f2 = std::move(tmp);
}


// helper functions
template <typename R, typename... Args>
template <typename FN_T>
void function<R(Args...)>::setHelperCallbacks() {
    // set the type of the invoker
    invoker_ = [](void* fn_holder, Args... args) -> R{
        FN_T* f = reinterpret_cast<FN_T*>(fn_holder);
        return (*f)(args...);
    };

    // set the fn_alloc_
    fn_alloc_ = [](void* fn_holder) -> void*{
        #ifdef DEBUG
        std::cout << "Calling fn_holder" << std::endl;
        #endif
        FN_T* fn_ptr = reinterpret_cast<FN_T*>(fn_holder);
        return reinterpret_cast<void*>(new FN_T(*fn_ptr));
    };

    // set the deletor
    deleter_ = [](void* fn_holder) -> void{
        #ifdef DEBUG
        std::cout << "Calling deleter" << std::endl;
        #endif
        FN_T* f = reinterpret_cast<FN_T*>(fn_holder);
        delete f;
    };
}

}

