#pragma once

#include "move.hpp"
#include "forward.hpp"

// TODO: give a synopsys on typing features

/*
    Key insights:
    1) T&& works like a reference under the hood. But, it's a reference
    to an rvalue. We can also control what we see as an rvalue. For
    example: 

    void wrapper(T arg)      // Copy made HERE when wrapper is called
    void wrapper(T&& arg)    // No copy, arg is just a reference

    Another example:
    T&& forward(T&& v) { return static_cast<T&&>(v); }
    In this example, if the return were T, then
    the result would have been copied onto the stack frame
    of the caller. However, now a rvalue type reference
    is passed.

    Try to always use references when passing in parameters. However,
    only in the return type of the object was not created in the
    stack frame of the function.
*/

// Synopsys
/**
 * @brief Returns forwarded reference
 * 
 * @example
 * template<typename T>
    void wrapper(T&& arg) {
    Without forward: arg is always treated as lvalue
    process(dev_std::forward<T>(arg));  Preserves lvalue/rvalue-ness
    }
    * 
    */
// template <typename T>
// T&& def_std::forward(T&& v) noexcept;


/**
* @brief Returns rvalue casted reference
*/
// template <typename T>
// typename remove_reference<T>::type def_std::move(T&& val) noexcept;
