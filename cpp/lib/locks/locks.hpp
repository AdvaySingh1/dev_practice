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

/*
    Key insights:

    Atomics:
    divided into the following;
    1) Memory order definitions:
        ex: inline constexpr auto memory_order_relaxed = memory_order::relaxed;
    2) Lock free definitions
        ex: #define ATOMIC_BOOL_LOCK_FREE unspecified
    3) template <class T> struct atomic
    4) template <class T> struct atomic<integral>
    5) template <class T> struct atomic<T*>: ex int x = 42; atomic ptr = &x; 
    6) typedef atomic<bool>               atomic_bool;
    7) typedef struct atomic_flag
    8) void atomic_thread_fence(memory_order m) noexcept;


    std::atomic is a class template with partial specializations (e.g., for integral types and for pointers).
    When you write std::atomic<int>, the compiler instantiates atomic<T> with T=int and, by the template rules,
    selects the most specialized match—the integral specialization.



    0 = never lock-free (always uses a lock)
    1 = sometimes lock-free (depends on object alignment or size)
    2 = always lock-free (guaranteed hardware atomic).
    Example:


    #if ATOMIC_INT_LOCK_FREE == 2
        // int atomics are always hardware lock-free — super fast
    #else
        // int atomics may use a mutex — slower
    #endif



    How compiler picks up on these arch specficis:
    When you build or install Clang/GCC, it’s compiled with a target triple (like x86_64-linux-gnu, aarch64-apple-darwin, etc.).
    That target triple tells the compiler what CPU architecture and ABI to assume.
    The compiler’s atomic implementation tables (baked into its source) say, for that architecture, which atomic widths (1, 2, 4, 8 bytes) are lock-free.
    When you compile your program, it substitutes the right constants (__CLANG_ATOMIC_INT_LOCK_FREE, etc.) based on that target.
    If you cross-compile (e.g., for ARM on an x86 laptop), Clang picks values for the target CPU, not the machine you’re compiling on.
    So: compile-time detection, not runtime — determined entirely by the compiler’s target configuration.

    Target triple: <architecture>-<vendor>-<system>-<abi>
    Run clang -v to check
    Target: arm64-apple-darwin23.2.0




    Weak vs Strong compare-exchange:
    Strong:

    If *this == expected, swap atomically and return true
    Guaranteed to only fail if values don't match
    One attempt, deterministic

    Weak:

    Does the same comparison and swap
    BUT can spuriously fail even when *this == expected (returns false without swapping)
    Happens on architectures with LL/SC (load-linked/store-conditional) where context switches can cause failures
    Faster on some architectures, but needs retry loop

    Usage:
    cpp// Strong - use when not in a loop
    if (atomic.compare_exchange_strong(expected, desired)) { /* success

    // Weak - use in loops (common pattern)
    // while (!atomic.compare_exchange_weak(expected, desired)) {
        // Spurious failures just retry, no big deal
    // }
    // TLDR: Weak can randomly fail even when it should succeed (hardware limitation). Use weak in loops, strong for single attempts.
    
    Essentially, 
    No, strong allows context switches - it just retries internally until it succeeds or truly fails (values don't match). Weak exposes those retry failures to you.
    



    Example uses of the API:
    #include <atomic>
    #include <cstdint>

    // (3) atomic<T>: generic trivially-copyable type → load/store/exchange/compare_exchange
    struct Pair { int a; int b; };
    std::atomic<Pair> ap{Pair{1,2}};
    void ex_generic() {
        Pair old = ap.load(std::memory_order_acquire);
        Pair expected = old, desired{old.a+1, old.b};
        ap.compare_exchange_strong(expected, desired, std::memory_order_acq_rel);
        ap.store(desired, std::memory_order_release);
    }

    // (4) atomic<integral>: adds fetch_add/sub & bitwise ops
    std::atomic<int> ai{0};
    void ex_integral() {
        ai.fetch_add(5, std::memory_order_relaxed); // 0→5
        ai.fetch_or(0x1, std::memory_order_acq_rel);
    }

    // (5) atomic<T*>: pointer arithmetic via fetch_add/fetch_sub
    int buf[8]{};
    std::atomic<int*> p{buf};
    void ex_ptr() {
        int* prev = p.fetch_add(2, std::memory_order_acq_rel); // advance by 2 elements
        *prev = 42; // writes buf[0] but this is not atomic
    }

    There's also these: template<class T>
    void atomic_store(atomic<T>*, atomic<T>::value_type) noexcept;
    template<class T>
    void atomic_store_explicit(volatile atomic<T>*, atomic<T>::value_type,
                                memory_order) noexcept;

    // (6) atomic_bool: boolean atomics (test/store/exchange/compare_exchange)
    std::atomic_bool ready{false};
    void ex_abool() {
        ready.store(true, std::memory_order_release);
        bool was = ready.exchange(false, std::memory_order_acq_rel);
    }

    // (7) atomic_flag: minimal test_and_set/clear (spinlock sketch)
    std::atomic_flag lock_flag = ATOMIC_FLAG_INIT;
    void lock()   { while (lock_flag.test_and_set(std::memory_order_acquire)) {} }
    void unlock() { lock_flag.clear(std::memory_order_release); }

    // (8) atomic_thread_fence: order non-atomic data around an atomic handshake
    int data = 0;                   // non-atomic payload
    std::atomic_bool done{false};   // publication flag
    void producer() {
        data = 123;
        std::atomic_thread_fence(std::memory_order_release);
        done.store(true, std::memory_order_relaxed);
    }
    int consumer() {
        while (!done.load(std::memory_order_relaxed)) {}
        std::atomic_thread_fence(std::memory_order_acquire);
        return data; // safely sees 123
    }


    acq_rel vs seq_cst
    This relates to the interconnects consistancy model. Essentially, acq_rel doesn't can't
    reorder instructions around this instruction, flush the store buffer but can issue other memory commands after
    completing that one. In the case of seq_cst, however, when you have a store, you flush
    the store buffer and wait for the other processors to ack that they've invalidated
    the piece of mem (assuming inv protocol like MOESI) before issuing further insts.
    Essentially: acq_rel: Does flush store buffer on release (stores become visible)
                 seq_cst: Flushes store buffer AND waits for global visibility (Acks)


    In dir protocols, say you do a write 1 with acq_rel and flush the store buffer.
    When you do the next write and flush the store buffer. The messages might reach
    another processor out of order, causing them to see the second one before the
    first one. Hence, this requires seq_cst.

    The reason you need it for the loads is due to the dir protocol in question.
    If the consistancy/coherence protocol has acks coming from the home directory,
    or the central bus acting as the consistancy fence, then it can send the sender
    acks itself, and the invalidations might arrive at the other processors out of
    order. Example:

    // Thread 1 (on Core 0)
    x.store(1, seq_cst);  // Message Mx sent at time T
    y.store(1, seq_cst);  // Message My sent at time T+1

    // Thread 2 (on Core 1)
    // Messages might arrive: My before Mx due to network routing/congestion!
    if (y.load(acq_rel) == 1) {  // Sees My first
        assert(x.load(acq_rel) == 1);  // Mx hasn't arrived yet - FAIL
    }
*/

