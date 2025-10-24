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
        // TODO: call volatile but when? before noexcept
        void lock() noexcept;
        void unlock() noexcept;
    };
}