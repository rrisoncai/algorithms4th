#include <dsp/Conv1d.hpp>

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
    static_assert(!std::is_copy_constructible<Conv1d::State>::value);
    static_assert(!std::is_copy_assignable<Conv1d::State>::value);

    bool threw = false;
    try {
        Conv1d empty({});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    const Conv1d conv({0.25f, 0.2f, 0.15f, 0.1f});
    auto state = conv.make_state();
    assert(near(conv.process(state, 1.0f), 0.25f));
    assert(near(conv.process(state, 2.0f), 0.70f));
    assert(near(conv.process(state, 3.0f), 1.30f));
    assert(near(conv.process(state, 4.0f), 2.00f));
    assert(near(conv.process(state, 5.0f), 2.70f));

    const Conv1d other_conv({1.0f});
    auto incompatible_state = other_conv.make_state();
    threw = false;
    try {
        conv.process(incompatible_state, 1.0f);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);

    const Conv1d passthrough({1.0f});
    auto shared_state = passthrough.make_state();
    std::vector<float> outputs(8, 0.0f);
    std::vector<std::thread> threads;
    threads.reserve(outputs.size());

    for (std::size_t i = 0; i < outputs.size(); ++i) {
        threads.emplace_back([&passthrough, &shared_state, &outputs, i]() {
            outputs[i] = passthrough.process(shared_state, static_cast<float>(i));
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (std::size_t i = 0; i < outputs.size(); ++i) {
        assert(near(outputs[i], static_cast<float>(i)));
    }

    auto state_a = conv.make_state();
    auto state_b = conv.make_state();
    float output_a = 0.0f;
    float output_b = 0.0f;

    std::thread thread_a([&]() { output_a = conv.process(state_a, 2.0f); });
    std::thread thread_b([&]() { output_b = conv.process(state_b, 4.0f); });
    thread_a.join();
    thread_b.join();

    assert(near(output_a, 0.5f));
    assert(near(output_b, 1.0f));

    return 0;
}
