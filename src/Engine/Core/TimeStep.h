#pragma once

#include <chrono>

namespace TinyTactics
{
    // Frame time data produced by Application and consumed by Layer updates.
    // Flow: Application::Run -> TimeStep -> Layer::OnUpdate.
    // Jump: future fixed timestep or animation systems should read from this type first.
    class TimeStep
    {
    public:
        using Clock = std::chrono::steady_clock;
        using Duration = std::chrono::duration<float>;

        TimeStep(Duration deltaTime = Duration::zero(), Duration elapsedTime = Duration::zero())
            : m_DeltaTime(deltaTime), m_ElapsedTime(elapsedTime)
        {
        }

        // Returns the time between the current frame and the previous frame.
        float GetSeconds() const
        {
            return m_DeltaTime.count();
        }

        // Returns the current frame delta in milliseconds for diagnostics/UI.
        float GetMilliseconds() const
        {
            return GetSeconds() * 1000.0F;
        }

        // Returns how long the application has been running.
        float GetElapsedSeconds() const
        {
            return m_ElapsedTime.count();
        }

        float GetElapsedMilliseconds() const
        {
            return GetElapsedSeconds() * 1000.0F;
        }

        // Keeps old Layer code usable where a float delta time is expected.
        operator float() const
        {
            return GetSeconds();
        }

    private:
        Duration m_DeltaTime = Duration::zero();
        Duration m_ElapsedTime = Duration::zero();
    };
}
