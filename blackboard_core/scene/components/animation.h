#pragma once
#include <bx/easing.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <algorithm>

namespace blackboard::core {

using Easing = bx::Easing::Enum;

namespace components {

struct Animation_entities
{
    operator const std::vector<entt::entity> &() const
    {
        return entities;
    }
    std::vector<entt::entity> entities;
};

class Animation
{
public:
    struct Info
    {
        float start_time{0.0f};
        float end_time{1000.0f};
        bool loop{false};
        bool ping_pong{false};
        ;
        Easing m_easing{Easing::Linear};
    };

private:
    struct Animation_concept
    {
        virtual ~Animation_concept()
        {}
        virtual void update(const float delta_t) = 0;
        virtual std::unique_ptr<Animation_concept> clone() const = 0;

        void reset()
        {
            m_current_time = 0.0f;
        }

        Info info;

        bool completed{false};
        bool active{true};

        float m_current_time{info.start_time};
        bx::EaseFn m_easing_fn{bx::easeLinear};
        bool forward{true};
    };

    template<typename T>
    struct Animation_model : public Animation_concept
    {
        T &value;

        T from;
        T to;

        Animation_model(T &v, T &&from, T &&to, Info &&anim_info)
            : value{std::forward<T &>(v)}, from{std::forward<T>(from)}, to{std::forward<T>(to)}
        {
            info = std::forward<Info>(anim_info);
        }

        std::unique_ptr<Animation_concept> clone() const override
        {
            return std::make_unique<Animation_model>(*this);
        }

        void update(const float delta_t) override
        {
            if (!active)
            {
                return;
            }

            if (completed && info.ping_pong)
            {
                forward = !forward;
            }

            if (m_current_time < info.end_time || !info.loop)
            {
                m_current_time = std::clamp(m_current_time + delta_t, info.start_time, info.end_time);
                const auto interp = m_easing_fn(glm::clamp(
                  (m_current_time - info.start_time) / (info.end_time - info.start_time), 0.0f, 1.0f));
                value = Animation::update(from, to, forward ? interp : 1.0f - interp);
                completed = false;
                return;
            }

            completed = true;
            reset();
        }
    };

    std::unique_ptr<Animation_concept> p_impl;

public:
    template<typename T>
    Animation(T &value, T &&from, T &&to, Info &&info)
        : p_impl{new Animation_model<T>(value, std::move(from), std::move(to), std::move(info))}
    {}

    Animation(const Animation &other): p_impl(other.p_impl->clone())
    {}

    Animation &operator=(const Animation &other)
    {
        p_impl = other.p_impl->clone();
        return *this;
    }

    template<typename T>
    static T update(const T &start_value, const T &end_value, const float interp);

    void update(const float delta_t)
    {
        p_impl->update(delta_t);
    }
};

//
//template<typename T>
//struct Animation : public Animation_tag
//{
//    using value_type = T;
//
//    Animation() = delete;
//    Animation(const float ms, const T &start, const T &end, const Easing &&easing = Easing::Linear)
//        : m_end_time(ms), m_start_value(start), m_end_value(end)
//    {
//        set_easing(std::move(easing));
//    }
//
//    decltype(auto) start_value() const
//    {
//        return m_start_value;
//    }
//
//    decltype(auto) end_value() const
//    {
//        return m_end_value;
//    }
//
//    auto start_value() -> T&
//    {
//        return m_start_value;
//    }
//
//    auto end_value() -> T&
//    {
//        return m_end_value;
//    }
//
//    float current_normalized_time() const
//    {
//        return m_easing_fn(glm::clamp(m_current_time / m_end_time, 0.0f, 1.0f));
//    }
//
//    float current_time() const
//    {
//        return m_current_time;
//    }
//
//    const T &current_value() const
//    {
//        return m_current_value;
//    }
//
//    void reset()
//    {
//        m_current_time = 0.0f;
//    }
//
//    void reset(const Easing &&easing)
//    {
//        set_easing(std::move(easing));
//        reset();
//    }
//
//    void set_easing(const Easing &&easing)
//    {
//        m_easing = easing;
//        m_easing_fn = bx::getEaseFunc(easing);
//    }
//
//    const Easing &get_easing() const
//    {
//        return m_easing;
//    }
//
//    void tick(float delta)
//    {
//        if(!active)
//        {
//            return;
//        }
//
//        if(completed && ping_pong)
//        {
//            forward = !forward;
//        }
//
//        if (m_current_time < m_end_time || !loop)
//        {
//            m_current_time = std::max(std::min(m_current_time + delta, m_end_time), 0.0f);
//            update();
//            completed = false;
//            return;
//        }
//
//        completed = true;
//        reset();
//    }
//
//    bool loop{false};
//    bool ping_pong{false};
//    bool completed{false};
//    bool active{false};
//
//private:
//    void update();
//
//    T m_start_value;
//    T m_end_value;
//    T m_current_value;
//    float m_current_time{0.0f};
//    float m_end_time{0.0f};
//    Easing m_easing{Easing::Linear};
//    bx::EaseFn m_easing_fn{bx::easeLinear};
//    bool forward{true};
//};

}    // namespace components
}    // namespace blackboard::core
