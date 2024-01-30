#ifndef VVR_ANIMATION_H
#define VVR_ANIMATION_H

#include "macros.h"
#include "utils.h"
#include "vvrframework_DLL.h"

namespace {

template <typename T>
bool
normalize(T &v)
{
  return v.Normalize();
}

template <>
inline bool
normalize(float &v)
{
  v = 1.0f;
  return true;
}

template <>
inline bool
normalize(double &v)
{
  v = 1.0;
  return true;
}

template <typename T>
float
overshoot(T const &a, T const &b)
{
  return a.Dot(b) < 0;
}

template <>
float
overshoot(float const &a, float const &b)
{
  return (b - a) < 0;
}

} // namespace

namespace vvr
{
    struct Animation
    {
        Animation()
            : m_paused(true)
            , m_time(0)
            , m_speed(1)
        {
        }

        float t() const
        {
            return m_time;
        }

        void pause()
        {
            m_paused = true;
        }

        bool paused() const
        {
            return m_paused;
        }

        void setSpeed(float speed)
        {
            m_speed = speed;
        }

        float speed()
        {
            return m_speed;
        }

        bool toggle()
        {
            if (m_paused) update(true);
            else pause();
            return !m_paused;
        }

        float update(bool start=false)
        {
            float sec;
            if (m_paused) {
                if (!start) return 0;
                m_last_update = sec = get_seconds();
            } else sec = get_seconds();
            const float delta_time = (sec - m_last_update) * m_speed;
            m_time += delta_time;
            m_last_update = sec;
            m_paused = false;
            return delta_time;
        }

        void setTime(float time)
        {
            m_time = time;
            m_last_update = get_seconds();
        }

        void reset()
        {
            pause();
            setTime(0);
        }

    private:
        bool m_paused;
        float m_time;
        float m_last_update;
        float m_speed;
    };

    template <typename P>
    struct PropertyAnimation : Animation
    {
        vvr_decl_shared_ptr(PropertyAnimation)

        PropertyAnimation() : prop(nullptr)
        {
        }

        ~PropertyAnimation()
        {
            terminate();
        }

        PropertyAnimation(const P &from, const P &to, P &prop, float dur = 1.0f)
          : from(from), to(to), d((to - from) / dur), prop(&prop), dur(dur)
        {
          animate();
        }

        float remaining() const { return dur - t(); }

        bool animate()
        {
          if (!prop) return false;
          update(true);
          const bool alive = t() < dur;
          if (alive) {
            *prop = from + (d * t());
            } else
              terminate();
            return alive;
        }

        void setPixelSpeed(float s)
        {
            const float dl = static_cast<float>(d.Length());
            setSpeed(s / dl / dur);
        }

        void terminate()
        {
          *prop = to;
          // reset();
        }

      private:
        P from, to, d;
        P *prop;
        float dur;
    };

    template <typename P>
    struct TargetAnimation : Animation
    {
      vvr_decl_shared_ptr(TargetAnimation)

        TargetAnimation(float vel = 1.0f)
        : dv{0.0f}, vel(vel)
      {}

      TargetAnimation(P const &value, float vel)
        : value(value), target(value), dv{0.0f}, vel(vel)
      {}

      ~TargetAnimation() { value = target; }

      void setTarget(P const &newTarget)
      {
        target = newTarget;
        auto dv1 = (target - value);

        if (normalize(dv1)) {
          dv = dv1 * vel;
        } else {
          dv = P{0.0f};
        }

        if (!paused()) animate();
      }

      void setValue(P const &newValue)
      {
        value = newValue;
        target = newValue;
        dv = P{0.0f};
      }

      void snapToTarget()
      {
        value = target;
        dv = P{0.0f};
      }

      bool animate()
      {
        const float dt = update(true);
        const P newValue = value + dv * dt;

        if (overshoot(dv, target - newValue)) {
          dv = P{0.0f};
          value = target;
          return false;
        } else {
          value = newValue;
          return true;
        }
      }

      P const &get() const { return value; }

    private:
      P value;
      P target;
      P dv;
      float vel;
    };
}

#endif
