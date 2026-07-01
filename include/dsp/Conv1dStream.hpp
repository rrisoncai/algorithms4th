#pragma once
#include <vector>
#include <mutex>
#include <stdexcept>
#include <cstddef>

class Conv1dStream {
    public:
    explicit Conv1dStream(const std::vector<float>& kernel)
    : h_(kernel),
    buf_(kernel.size(), 0),
    head_(0)
    {
        if (kernel.empty()) {
            throw std::invalid_argument("kernel must not be empty");
        }
    }
    Conv1dStream(const Conv1dStream&) = delete;
    Conv1dStream& operator=(const Conv1dStream&) = delete;
    float process(float x_new) {
        std::lock_guard<std::mutex> lock(mutex_);
        const std::size_t N = h_.size();

        buf_[head_] = x_new;

        float y = 0.0f;
        std::size_t idx = head_;

        for (std::size_t k = 0; k < N; ++k) {
            y += h_[k] * buf_[idx];

            idx = (idx + N - 1) % N;
        }

        head_ = (head_ + 1) % N;

        return y;
    }

    private:
    const std::vector<float> h_;
    std::vector<float> buf_;
    std::size_t head_;
    std::mutex mutex_;
};
