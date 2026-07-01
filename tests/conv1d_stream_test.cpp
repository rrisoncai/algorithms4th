#include <dsp/Conv1dStream.hpp>

#include <cassert>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

namespace {

bool near(float a, float b)
{
    return std::fabs(a - b) < 1e-6f;
}

} // namespace

int main()
{
    static_assert(!std::is_copy_constructible<Conv1dStream>::value);
    static_assert(!std::is_copy_assignable<Conv1dStream>::value);

    bool threw = false;
    try {
        Conv1dStream empty({});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    Conv1dStream conv({0.25f, 0.2f, 0.15f, 0.1f});
    assert(near(conv.process(1.0f), 0.25f));
    assert(near(conv.process(2.0f), 0.70f));
    assert(near(conv.process(3.0f), 1.30f));
    assert(near(conv.process(4.0f), 2.00f));
    assert(near(conv.process(5.0f), 2.70f));

    Conv1dStream passthrough({1.0f});
    std::vector<float> outputs(8, 0.0f);
    std::vector<std::thread> threads;
    threads.reserve(outputs.size());

    for (std::size_t i = 0; i < outputs.size(); ++i) {
        threads.emplace_back([&passthrough, &outputs, i]() {
            outputs[i] = passthrough.process(static_cast<float>(i));
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (std::size_t i = 0; i < outputs.size(); ++i) {
        assert(near(outputs[i], static_cast<float>(i)));
    }

    return 0;
}
