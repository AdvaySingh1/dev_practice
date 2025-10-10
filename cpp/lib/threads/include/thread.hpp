#pragma once
#include <pthread.h>
#include <functional>
#include <utility>
#include <atomic>
#include <cstdint>

namespace os {

class Thread {
public:
  using Entry = std::function<void()>;

  Thread() noexcept = default;
  explicit Thread(Entry fn);
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;
  Thread(Thread&& other) noexcept;
  Thread& operator=(Thread&& other) noexcept;
  ~Thread();

  // Starts the thread if not already started.
  // R: fn_ is set; thread not started yet.
  // M: internal pthread handle, started_ flag.
  // E: Creates an OS thread running fn_. Throws on error.
  void start();

  // Blocks until thread finishes.
  // R: Thread must have been started and not yet joined; not detached.
  // M: joined_ flag.
  // E: Waits for completion; invalid to call twice. Throws on error.
  void join();

  // Detach underlying pthread.
  // R: Thread must have been started and not joined/detached.
  // M: detached_ flag.
  // E: Releases joinable resource; thread continues independently.
  void detach();

  // Returns true if joinable (started and not joined/detached).
  // R: none.
  // M: none.
  // E: Pure query.
  bool joinable() const noexcept;

  // Get native handle (undefined if not started).
  // R: started_ == true.
  // M: none.
  // E: Returns pthread_t for low-level ops.
  pthread_t native_handle() const noexcept { return handle_; }

private:
  static void* trampoline_(void* arg);

  Entry fn_{};
  pthread_t handle_{};
  std::atomic<bool> started_{false};
  std::atomic<bool> joined_{false};
  std::atomic<bool> detached_{false};
};

} // namespace os
