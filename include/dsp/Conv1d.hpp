#pragma once

#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

class Conv1d {
public:
    class State {
    public:
        State(const State&) = delete;
        State& operator=(const State&) = delete;

    private:
        friend class Conv1d;

        explicit State(std::size_t kernel_size)
            : buffer_(kernel_size, 0.0f)
        {
        }

        std::vector<float> buffer_;
        std::size_t head_ = 0;
        std::mutex mutex_;
    };

    explicit Conv1d(std::vector<float> kernel)
        : kernel_(std::move(kernel))
    {
        if (kernel_.empty()) {
            throw std::invalid_argument("kernel must not be empty");
        }
    }

    [[nodiscard]]
    State make_state() const
    {
        return State(kernel_.size());
    }

    // kernel_[0] multiplies the newest sample; each State holds one stream.
    float process(State& state, float x_new) const
    {
        std::lock_guard<std::mutex> lock(state.mutex_);
        const std::size_t kernel_size = kernel_.size();

        if (state.buffer_.size() != kernel_size) {
            throw std::invalid_argument("state does not match kernel size");
        }

        state.buffer_[state.head_] = x_new;

        float y = 0.0f;
        std::size_t idx = state.head_;

        for (std::size_t k = 0; k < kernel_size; ++k) {
            y += kernel_[k] * state.buffer_[idx];
            idx = (idx + kernel_size - 1) % kernel_size;
        }

        state.head_ = (state.head_ + 1) % kernel_size;

        return y;
    }

private:
    std::vector<float> kernel_;
};
