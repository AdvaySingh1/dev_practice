#include <atomic>


// synopsys

namespace dev_std {
    /**
     * @brief Test and Set Lock
     * 
     */
    class testAndSetLock {
        private:
            std::atomic_bool lock = 0; 
        public:
        void lock() noexcept;
        void unlock() noexcept;
    };
}