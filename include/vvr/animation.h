#ifndef VVR_ANIMATION_H
#define VVR_ANIMATION_H

#include "vvrframework_DLL.h"

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
            , m_time(0)
            , m_speed(1)
            , m_paused(true)
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

        void update(bool force_resume = false)
        {
            const float sec = getSeconds();
            if (m_paused) if (force_resume) m_last_update = sec; else return;
            m_paused = false;
            m_time += ((sec - m_last_update) * m_speed);
            m_last_update = sec;
        }

        void setTime(float time)
        {
            const float sec = getSeconds();
            m_time = time;
            m_last_update = sec;
        }

    private:
        bool m_paused;
        float m_time;
        float m_last_update;
        float m_end_time;
        float m_speed;
    };

}

#endif
