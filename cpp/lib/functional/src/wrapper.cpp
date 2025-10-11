#include "wrapper.hpp"

namespace dev_std {

    // copy ctor
    template <typename R, typename... Args>
    function<R(Args...)>::function(const function&) {
        
    }


    // copy ctor
    template <typename R, typename... Args>
    function<R(Args...)>::function(function&&) noexcept {
        
    }


}