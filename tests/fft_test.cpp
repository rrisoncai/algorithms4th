#include <dsp/fft.hpp>

#include <cassert>
#include <cmath>
#include <complex>
#include <stdexcept>
#include <vector>

namespace {

using Complex = std::complex<float>;

std::vector<Complex> dft(const std::vector<Complex>& input)
{
    const int n = input.size();
    const float pi = std::acos(-1.0f);
    std::vector<Complex> output(n, Complex(0.0f, 0.0f));

    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            const float angle = -2.0f * pi * k * i / n;
            output[k] += input[i] * std::exp(Complex(0.0f, angle));
        }
    }

    return output;
}

} // namespace

int main()
{
    std::vector<Complex> single{{2.0f, -3.0f}};
    fft(single);
    assert(single[0] == Complex(2.0f, -3.0f));

    std::vector<Complex> empty;
    bool empty_threw = false;
    try {
        fft(empty);
    } catch (const std::invalid_argument&) {
        empty_threw = true;
    }
    assert(empty_threw);

    std::vector<Complex> x;
    for (int i = 0; i < 16; ++i) {
        x.push_back(Complex(i / 10.0f, i / 5.0f));
    }

    std::vector<Complex> expected = dft(x);
    fft(x);

    for (int i = 0; i < 16; ++i) {
        assert(std::abs(x[i] - expected[i]) < 1e-4f);
    }

    std::vector<Complex> bad_size = {Complex(1.0f, 0.0f), Complex(2.0f, 0.0f), Complex(3.0f, 0.0f)};
    bool threw = false;
    try {
        fft(bad_size);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    return 0;
}
