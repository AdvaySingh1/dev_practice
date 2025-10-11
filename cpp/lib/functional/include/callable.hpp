#pragma once
#include <functional> <- reference

namespace dev_std {
// forward decl
template <typename Ret, typename... Args>
class callable;


template <typename Ret, typename... Args>
class callable<Ret(Args...)> {
    private:
        void* callable_ = nullptr;

        RET (*invoker_)(void*, Args...) = nullptr;

        void (*deleter_)(void*) = nullptr;

    public:
        // defualt ctor: initialize everything to nullptr
        callable() = default;

        /*
        std::move() since many functors, and lambda functions capture
        rvalues (such as unique pointer).
        Additionally, the move function means that this callable object
        now owns the resources which originally belonged to the closure.
        Hence, we must have a deleter
        */
        template<typename FN_T>
        callable(FN_T F) 
        : callable_(reinterpret_cast<void*>(new FN_T(std::move(F)))) { // allocated in the heap so it stays in scope


            invoker_ = [](void* callable_ptr, Args... args) -> Ret{ // not take it as an arg
                FN_T* f = reinterpret_cast<FN_T*>(callable_ptr);
                return (*f)(args...);
                /*
                In the case that FN_T is a function pointer type, then we can simply call
                f(args...). However, in the case of an object with closure, we need
                (*f)(args...);
                */
            };
            /*
            The following doesn't work since lambda functions and only be stored as function pointers
            in the case that there's no data capture. Otherwise, the operator overload requires extra-params
            being passed in which is resolved at compile time.
            invoker_ = [callable_](Args... args) -> Ret{
                FN_T* f = static_cast<FN_T*>(callable_);
                return (*f)(args...);
            };
            */

            // here, somehow check if callable_ is allocated on the heap?
            deleter_ = [](void* callable_ptr) -> void{
                FN_T* f = reinterpret_cast<FN_T*>(callable_ptr);
                delete f;
            }
        }

        ~callable() {
            // check if deleter_ since callable class might be default initialized
            if (deleter_) deleter_(callable_); // same as (*deleter)(callable_);
        }

        // function invocation
        Ret operator ()(Args... args) {
            return invoker_(callable_, args...); // same as (*invoker_)(callable_, args...);
        }

        // function capacity
        explicit operator bool () const noexcept {
            return callable_ != nullptr;
        }

        // TODO
        callable& operator=(const callable& other) {}
        callable& operator=(callable&& other) noexcept {}
        callable& operator=(nullptr_t) noexcept {}

        template <typename FN_T>
        callable& operator=(const FN_T& f) {}

        template <typename FN_T>
        callable& operator=(FN_T&& f) noexcept {}

        void swap(callable& other) {}




};

    // nullptr comparison
    template <typename Ret, typename... Args>
    bool operator==(callable<Ret(Args...)> c, nullptr_t) noexcept {
         return !static_cast<bool>(c); 
    }

    template <typename Ret, typename... Args>
    bool operator==(nullptr_t, callable<Ret(Args...)> c) noexcept {
         return !static_cast<bool>(c); 
    }

    template <typename Ret, typename... Args>
    bool operator!=(callable<Ret(Args...)> c, nullptr_t) noexcept {
         return static_cast<bool>(c); 
    }

    template <typename Ret, typename... Args>
    bool operator!=(nullptr_t, callable<Ret(Args...)> c) noexcept {
         return static_cast<bool>(c); 
    }

}