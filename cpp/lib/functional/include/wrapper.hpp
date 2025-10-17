#pragma once
#include <cstddef>
#include "types.hpp"

#ifdef DEBUG
#include <iostream>
#endif


namespace dev_std {


template <typename R, typename... Args>
class function;

// synopsys
template <typename R, typename... Args>
class function<R(Args...)> {
    private: // QUES: should these be in copy? Would there ever be a class which wants to inherit from this?
    // function holder
    void* fn_holder_ = nullptr;
    // function invoker
    R (*invoker)(void*, Args...) = nullptr;
    // function deleter
    void (*deleter)(void*) = nullptr;

    public:
    // default ctor, init to nullptr
    function() = default;
    // copy ctor. Copy the function and alloc onto heap
    // function(const function&);
    // move ctor. Move the function and alloc onto heap
    // function(function&&) noexcept;
    // copy function ctor. Copy the function and alloc onto heap
    template <typename FN_T>
    function(const FN_T&);
    // move function ctor. Move the function and alloc onto heap
    template <typename FN_T>
    function(FN_T&&) noexcept;
    // assignment operator
    // function& operator=(const function&);
    // function& operator=(function&&) noexcept;

    // capacity operator
    [[nodiscard]] explicit operator bool() const noexcept;

    // comparison operator
    // [[nodiscard]] bool operator==(const function&) const noexcept;

    // invoker
    R operator()(Args...) const;

    // destructor
    ~function();

    // swap operator
    // void swap(function&);


    private:
    // helper functions

    template <typename FN_T>
    void set_invoker_and_deleter();


};

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

// template <typename R, typename... Args>
// function<R(Args...)>::function(const function& F) {
//     // TODO
// }

// template <typename R, typename... Args>
// function<R(Args...)>::function(function&& F) noexcept {
//     // TODO
// }

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
    set_invoker_and_deleter<FN_T>();

    #ifdef DEBUG
    std::cout << "lvalue ctor called" << std::endl;
    #endif

}

template <typename R, typename... Args>
template <typename FN_T>
function<R(Args...)>::function(FN_T&& F) noexcept {
    // copy the function object onto the heap
    fn_holder_ = reinterpret_cast<void*>(new FN_T(dev_std::move(F))); // ensure rvalue

    // set the invoker and deleter based on the type
    set_invoker_and_deleter<FN_T>();

    #ifdef DEBUG
    std::cout << "rvalue ctor called" << std::endl;
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
    return invoker(fn_holder_, args...);
}

template <typename R, typename... Args>
function<R(Args...)>::~function() {
    if (*this) { // check if fn_holder_ is nullptr
        deleter(fn_holder_);
    }
}




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



// helper functions

template <typename R, typename... Args>
template <typename FN_T>
void function<R(Args...)>::set_invoker_and_deleter() {
    // set the type of the invoker
    invoker = [](void* fn_holder, Args... args) -> R{
        FN_T* f = reinterpret_cast<FN_T*>(fn_holder);
        return (*f)(args...);
    };

    // set the deletor
    deleter = [](void* fn_holder) -> void{
        FN_T* f = reinterpret_cast<FN_T*>(fn_holder);
        delete f;
    };
}

}

