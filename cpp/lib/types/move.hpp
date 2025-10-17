#pragma once

namespace dev_std {

    template <typename T>
    struct remove_reference {
        typedef T type;
    };

    template <typename T>
    struct remove_reference<T&> {
        typedef T type;
    };

    template <typename T>
    struct remove_reference<T&&> {
        typedef T type;
    };

    template <typename T>
    typename remove_reference<T>::type move(T&& val) noexcept {
        return static_cast<typename remove_reference<T>::type&&>(val);
    }




}