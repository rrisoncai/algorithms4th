#pragma once

#include <cstddef>
#include <complex>
#include <vector>
#include <stdexcept>
#include <cmath>

inline bool is_power_of_two(std::size_t n)
{
    return (n > 0) && (n & (n - 1)) == 0;
}

// Unnormalized forward FFT; input length must be a nonzero power of two.
inline void fft(std::vector<std::complex<float>>& x)
{
    const std::size_t n = x.size();

    if (!is_power_of_two(n)) {
        throw std::invalid_argument("fft input size must be power of two");
    }

    if (n <= 1) {
        return;
    }
    std::vector<std::complex<float>> even(n / 2, 0), odd(n / 2, 0);
    const float pi = std::acos(-1.0f);

    for (std::size_t k = 0; k < n / 2; ++k) {
        even[k] = x[2 * k];
        odd[k] = x[2 * k + 1];
    }

    fft(even);
    fft(odd);

    for (std::size_t k = 0; k < n / 2; ++k) {
        const float angle = -2.0f * pi * static_cast<float>(k) / static_cast<float>(n);
        const auto twiddle = std::exp(std::complex<float>(0.0f, angle));
        x[k] = even[k] + twiddle * odd[k];
        x[k + n / 2] = even[k] - twiddle * odd[k];
    }
}
