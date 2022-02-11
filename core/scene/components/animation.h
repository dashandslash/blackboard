#pragma once

namespace blackboard::components {

template<typename T>
struct Animation
{
    Animation() = delete;
    Animation(const float ms, const T &start, const T &end)
        : m_end_time(ms), m_start_value(start), m_end_value(end)
    {}

    decltype(auto) start_value() const { return m_start_value; }

    decltype(auto) end_value() const { return m_end_value; }

    float end_time() const { return m_end_time; }

    void tick(float delta)
    {
        curr_time += delta;

        if (curr_time < end_time() || !loop)
        {
            return;
        }

        curr_time = 0.0f;

        if (ping_pong)
        {
            std::swap(m_start_value, m_end_value);
        }
    }

    float curr_time{0.0f};
    bool loop{true};
    bool ping_pong{true};

private:
    float m_end_time{0.0f};
    T m_start_value;
    T m_end_value;
};

}    // namespace blackboard::components
