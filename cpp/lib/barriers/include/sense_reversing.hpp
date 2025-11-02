#include "types.hpp"
#include "wrapper.hpp"
#include <atomic>

#ifdef DEBUG
#include <iostream>
#endif

/**
 * @brief Barrier for synchronizing multiple threads
 * This is not thread safe.
 */
namespace dev_std {

template <typename CompFT> class senseReversingBarrier {
private:
  atomic_uint tExpected_ = 0;
  atomic_uint tCount_ = 0;
  // pass this in by reference so we don't have to create a copy
  CompFT completionFunc_ = CompFT();

  atomic_bool globalSense_ = false;

public:
  senseReversingBarrier() = default;

  /**
   * @brief Construct a new s r barrier object
   * Here we pass in ComFT f by value
   * This is since in the case of an Lvalue, we
   * want to copy it and in the case of an Rvalue
   * we want to aquire it in place (which this does)
   *
   *
   * Although this API requies
   * std::move to be called, if it were &&, then,
   * there could be cases when you want a function
   * to be copied but it's not. So you manaully
   * make a copy.
   *
   * @param tCount
   * @param f
   */
  senseReversingBarrier(uint32_t tCount, CompFT completionFunc)
      : tExpected_(tCount), tCount_(t_count), completionFunc_(completionFunc) {}

  void wait(bool localSense) {
    while (localSense != globalSense_.load(memory_order_aquire))
      ;
  }

  // should this be nexcept?
  bool arrive() {
    // create a local sense
    bool localSense = !globalSense_.load(memory_order_aquire);
    // just begin explicit with memory ordering
    // no more threads, flip the sense
    if (tCount_.fetch_sub(1, memory_order_seq_cst) == 1) {
      // call the completion func
      completionFunc_();
      // reset the counter
      tCount_.store(tExpected_.load(memory_order_relaxed),
                    memory_order_release);
      // flip the sense
      globalSense_.store(!globalSense_, memory_order_release);
    }
    return localSense;
  }

  /**
   * Things to consider:
   *
   * The drop function will never be called from the wait stage.
   * Hence, that specific thread will not have decreased it's
   * share of the tCount_. Hence, it's safe to stop the tCount_.
   * However, in the case that tCount_ is 1, (we are only waiting)
   * on this function, there's two possibilities:
   *
   * 1) Other threads are in the wait stage, waiting on this thread
   * 2) This is the only thread and no other threads are waiting
   *
   *
   */
  void drop() {
    // decrease the expected count
    tExpected_.fetch_sub(1, memory_order_seq_cst);

    // need to derease the tCount_ as well
    /*
        Things to consider:
        the drop function wil
    */

    if (t)
  }

  void arrive_and_wait() { wait(arrive()); }

  void arive_and_drop();
};
} // namespace dev_std

/*
    This doesn't gurantee sc because the messages can be re-ordered

*/