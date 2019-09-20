#ifndef _SPIN_LOCK_H_
#define _SPIN_LOCK_H_
#include <atomic>
class spin_lock {
  public:
    spin_lock() noexcept { m_lock.clear(); }
    void lock() {
        while (m_lock.test_and_set(std::memory_order_acquire))
            ;
    }
    void unlock() { m_lock.clear(std::memory_order_release); }
    spin_lock(const spin_lock &) = delete;
    spin_lock(spin_lock &&) = delete;
    spin_lock &operator=(const spin_lock &) = delete;

  private:
    std::atomic_flag m_lock;
};
#endif //_SPIN_LOCK_H_
