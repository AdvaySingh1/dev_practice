#include <atomic>


using namespace std;


#if ATOMIC_INT_LOCK_FREE == 2
    // int atomics are always hardware lock-free — super fast
#else
    // int atomics may use a mutex — slower
#endif