#pragma once

namespace dev_std {

#include "callable.hpp"
template <typename Ret, typename... Args>
class functional;

template <typename Ret, typename... Args>
class functional<Ret(Args...)> {
    private:
        typedef dev_std::f_<Ret(Args...)> func_t;
        func_t f_;
    public:
        functional() = default;

        template<FN_T>
        functional(FN_T F): f_(F);

        ~functional() = default;

        Ret operator (Args... args) const {return f_(args...)}

        bool operator == (const functional& other) const noexcept { return this == &other}
        // bool operator = (const functional& other) const { return this == &other}; <- shold we even be able to have this
        bool operator = (functional&& other) noexcept { f_(std::move(other.f_)) }

        void swap (functional& other) noexcept { std::swap(*this, other);}

        explicit operator bool() const noexcept {return f_ != nullptr; }





};

}
