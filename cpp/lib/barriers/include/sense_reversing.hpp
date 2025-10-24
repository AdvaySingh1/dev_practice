#include <atomic>
#include "wrapper.hpp"

#ifdef DEBUG
#include <iostream>
#endif


namespace dev_std {
    class s_r_barrier {
        private:
            uint32_t t_count_ = 0;
            function<void(void)> f_; //<- check the args for this

        public:
        s_r_barrier() = default;
        s_r_barrier(uint32_t t_count) : t_count_(t_count) {}
        s_r_barrier(uint32_t t_count, const function<void(void)>& f) 
            : t_count_(t_count), f_(f) {
                
            }

    };
