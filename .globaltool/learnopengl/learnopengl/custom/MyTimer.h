/** Copyright (C) 2010-2012 by Jason L. McKesson **/
/** This file is licensed under the MIT License. **/



#ifndef MYTIMER_H
#define MYTIMER_H

#include <math.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

namespace MyTimer {
	class Timer {
	public:
		enum class Type
		{
			TT_LOOP,
			TT_SINGLE,
			TT_INFINITE,

			NUM_TIMER_TYPES,
		};

		/**
		Creates a timer with the given type.

		LOOP and SINGLE timers need an explicit duration. This represents the time in seconds
		through a loop, or the time in seconds until the timer expires.

		INFINITE timers ignore the duration.

		It is legal to create these statically.
		**/
		Timer(Type eType = Type::TT_INFINITE, float fDuration = 1.0f)
			: m_eType(eType)
			, m_secDuration(fDuration)
			, m_hasUpdated(false)
			, m_isPaused(false)
			, m_absPrevTime(0.0f)
			, m_secAccumTime(0.0f)
		{
			if (m_eType != Type::TT_INFINITE)
				assert(m_secDuration > 0.0f);
		}

		//Resets the timer, as though the user just created the object with the original parameters.
		void Reset() {
			m_hasUpdated = false;
			m_secAccumTime = 0.0f;
		}

		//Pauses/unpauses. Returns true if the timer is paused after the toggling.
		bool TogglePause() {
			m_isPaused = !m_isPaused;
			return m_isPaused;
		}

		//Sets the pause state to the given value.
		void SetPause(bool pause = true) {
			m_isPaused = pause;
		}

		//Returns true if the timer is paused.
		bool IsPaused() const {
			return m_isPaused;
		}

		//Updates the time for the timer. Returns true if the timer has reached the end.
		//Will only return true for SINGLE timers that have reached their duration.
		bool Update() {
			//float absCurrTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
			float absCurrTime = (float)glfwGetTime();
			if (!m_hasUpdated)
			{
				m_absPrevTime = absCurrTime;
				m_hasUpdated = true;
			}

			if (m_isPaused)
			{
				m_absPrevTime = absCurrTime;
				return false;
			}

			float fDeltaTime = absCurrTime - m_absPrevTime;
			m_secAccumTime += fDeltaTime;

			m_absPrevTime = absCurrTime;
			if (m_eType == Type::TT_SINGLE)
				return m_secAccumTime > m_secDuration;

			return false;
		}

		//Subtracts secRewind from the current time and continues from there.
		void Rewind(float secRewind) {
			m_secAccumTime -= secRewind;
			if (m_secAccumTime < 0.0f)
				m_secAccumTime = 0.0f;
		}

		//Adds secRewind to the current time and continues from there.
		void Fastforward(float secFF) {
			m_secAccumTime += secFF;
		}

		//Returns a number [0, 1], representing progress through the duration. Only used
		//for SINGLE and LOOP timers.
		float GetAlpha() const 	{
			switch (m_eType)
			{
			case Type::TT_LOOP:
				return fmodf(m_secAccumTime, m_secDuration) / m_secDuration;
			case Type::TT_SINGLE:
				return glm::clamp(m_secAccumTime / m_secDuration, 0.0f, 1.0f);
			}

			return -1.0f;	//Garbage.
		}

		//Returns a number [0, duration], representing the progress through the timer in 
		//seconds. Only for SINGLE and LOOP timers.
		float GetProgression() const 	{
			switch (m_eType)
			{
			case Type::TT_LOOP:
				return fmodf(m_secAccumTime, m_secDuration);
			case Type::TT_SINGLE:
				return glm::clamp(m_secAccumTime, 0.0f, m_secDuration);
			}

			return -1.0f;	//Garbage.
		}

		//Returns the time in seconds since the timer was started, excluding
		//time for pausing.
		float GetTimeSinceStart() const {
			return m_secAccumTime;
		}

		//Returns the timer's duration that was passed in.
		float GetDuration() const {
			return m_secDuration;
		}

	private:
		Type m_eType;
		float m_secDuration;

		bool m_hasUpdated;
		bool m_isPaused;

		float m_absPrevTime;
		float m_secAccumTime;
	};
}


#endif //MYTIMER_H
