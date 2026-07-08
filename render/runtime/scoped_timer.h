#ifndef _RENDER_RUNTIME_SCOPED_TIMER_H_
#define _RENDER_RUNTIME_SCOPED_TIMER_H_

#include <chrono>

class ScopedTimer
{
public:
    explicit ScopedTimer(double& output_ms)
        : output_ms_(output_ms), start_(Clock::now())
    {
    }

    ~ScopedTimer()
    {
        output_ms_ = std::chrono::duration<double, std::milli>(Clock::now() - start_).count();
    }

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

private:
    using Clock = std::chrono::steady_clock;

    double& output_ms_;
    Clock::time_point start_;
};

#endif
