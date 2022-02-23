#pragma once
#include <bx/easing.h>
#include <glm/glm.hpp>

namespace blackboard {

using Easing = bx::Easing::Enum;

namespace components {

template<typename T>
struct Animation
{
    Animation() = delete;
    Animation(const float ms, const T &start, const T &end, const Easing &easing = Easing::Linear)
        : m_end_time(ms), m_start_value(start), m_end_value(end)
    {
        set_easing(easing);
    }

    decltype(auto) start_value() const { return m_start_value; }

    decltype(auto) end_value() const { return m_end_value; }

    float current_time() const
    {
        return m_easing_fn(glm::clamp(m_current_time / m_end_time, 0.0f, 1.0f));
    }

    const T &current_value() const
    {
        return m_current_value;
    }

    void reset()
    {
        m_current_time = 0.0f;
    }

    void set_easing(const Easing &easing)
    {
        m_easing = easing;
        m_easing_fn = bx::getEaseFunc(easing);
    }

    const Easing &get_easing() const
    {
        return m_easing;
    }

    void tick(float delta)
    {
        m_current_time += delta;
        update();

        if (m_current_time < m_end_time || !loop)
        {
            completed = false;
            return;
        }

        completed = true;
        reset();

        if (ping_pong)
        {
            std::swap(m_start_value, m_end_value);
        }
    }

    bool loop{false};
    bool ping_pong{false};
    bool completed{false};

private:
    void update();
    float m_current_time{0.0f};
    float m_end_time{0.0f};
    T m_start_value;
    T m_end_value;
    T m_current_value;
    Easing m_easing{Easing::Linear};
    bx::EaseFn m_easing_fn{bx::easeLinear};
};

}    // namespace components
}    // namespace blackboard
