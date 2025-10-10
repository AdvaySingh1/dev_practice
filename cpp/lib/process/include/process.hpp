#pragma once
#include <sys/types.h>
#include <signal.h>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

namespace os {

class Process {
public:
  Process() = default;

  // Configure what to exec: path, argv, env (optional), working dir (optional).
  // Call start() to actually fork/exec.
  Process& set_path(std::string path);   // RME below
  Process& set_args(std::vector<std::string> args);
  Process& set_env(std::vector<std::string> env);       // replaces environment
  Process& set_cwd(std::string cwd);

  // Forks and execs the configured program.
  // R: path_ is non-empty; binary exists and is executable.
  // M: pid_, internal state; in child: replaces image with new program.
  // E: On success, returns pid in parent; throws on error. Child does not return if exec succeeds.
  pid_t start();

  // Wait for process to exit; returns exit status (or signal).
  // R: Process has been started and not already waited.
  // M: internal state; consumes zombie via waitpid.
  // E: Blocks (or polls if WNOHANG variant implemented elsewhere). Throws on error.
  int wait();

  // Non-blocking check: is the process still running?
  // R: started_ == true.
  // M: may reap if already exited (depending on impl detail).
  // E: Returns true if alive.
  bool is_running() const;

  // Send a signal (default SIGTERM).
  // R: started_ == true.
  // M: OS process state (target).
  // E: Returns true on success, false if ESRCH; throws on invalid usage.
  bool kill(int sig = SIGTERM) const noexcept;

  // PID of child (0 if not started).
  // R: none.
  // M: none.
  // E: Pure query.
  pid_t pid() const noexcept { return pid_; }

private:
  std::string path_{};
  std::vector<std::string> args_{};
  std::optional<std::vector<std::string>> env_; // if unset, inherit
  std::optional<std::string> cwd_;
  pid_t pid_{0};
  bool started_{false};
  bool waited_{false};
};

} // namespace os
