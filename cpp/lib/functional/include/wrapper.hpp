#pragma once

#include <cstddef>

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
    function(const function&);
    // move ctor. Move the function and alloc onto heap
    function(function&&) noexcept;
    // copy function ctor. Copy the function and alloc onto heap
    template <typename FN_T>
    function(FN_T&);
    // move function ctor. Move the function and alloc onto heap
    template <typename FN_T>
    function(FN_T&&);
    // assignment operator
    function& operator=(const function&);
    function& operator=(function&&) noexcept;

    // capacity operator
    [[nodiscard]] explicit operator bool() const noexcept;

    // comparison operator
    [[nodiscard]] explicit bool operator==(const function&) const noexcept;

    // invoker
    R operator()(Args...) const;

    // destructor
    ~function();

    // swap operator
    void swap(function&);

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


template <typename R, typename... Args, typename FN_T>
function<R(Args...)>::(FN_T&);

// copy ctor
template <typename R, typename... Args>
function<R(Args...)>::function(const function&) {
    
}

// move ctor
template <typename R, typename... Args>
function<R(Args...)>::function(function&&) noexcept {
    
}



}