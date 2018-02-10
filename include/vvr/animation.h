#ifndef VVR_ANIMATION_H
#define VVR_ANIMATION_H

#include "vvrframework_DLL.h"
#include "utils.h"

namespace vvr
{
    struct Animatable
    {
        virtual bool animate(float time) = 0;
        virtual float getTotalDuration() { return 0; }
    };

    struct Animation
    {
        Animation()
            : t(m_time)
            , m_paused(true)
            , m_time(0)
            , m_speed(1)
        {
        }

        const float &t;

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

        bool update(bool start=false)
        {
            float sec;
            if (m_paused) {
                if (!start) return false;
                m_last_update = sec = getSeconds();
            } else sec = getSeconds();
            m_time += ((sec - m_last_update) * m_speed);
            m_last_update = sec;
            m_paused = false;
            return true;
        }

        void setTime(float time)
        {
            m_time = time;
            m_last_update = getSeconds();
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

}

#endif
