
#pragma once
#include <chrono>
#include <type_traits>

namespace inceptionengine
{
	class Timer
	{
	public:
		struct Second
		{
			using Type = float;
		};
		struct Millisecond
		{
			using Type = long long;
		};
		struct Nanosecond
		{
			using Type = long long;
		};

	public:
		Timer()
		{
			mStartTime = mLastFrameTime = mNowTime = std::chrono::high_resolution_clock::now();
		}

		void Reset()
		{
			mStartTime = mLastFrameTime = mNowTime = std::chrono::high_resolution_clock::now();
		}

		template<typename T>
		typename T::Type GetDeltaTime()
		{
			static_assert(std::is_same_v<T, Timer::Second> || 
						  std::is_same_v<T, Timer::Millisecond> ||
						  std::is_same_v<T, Timer::Nanosecond>);

			if constexpr (std::is_same_v<T, Timer::Second>)
			{
				return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(mNowTime - mLastFrameTime).count()) / 1e9f;
			}
			else if constexpr (std::is_same_v<T, Timer::Millisecond>)
			{
				return std::chrono::duration_cast<std::chrono::milliseconds>(mNowTime - mLastFrameTime).count();
			}
			else if constexpr (std::is_same_v<T, Timer::Nanosecond>)
			{
				return std::chrono::duration_cast<std::chrono::nanoseconds>(mNowTime - mLastFrameTime).count();
			}
		}

		template<typename T>
		typename T::Type GetRunningTime()
		{
			static_assert(std::is_same_v<T, Timer::Second> ||
						  std::is_same_v<T, Timer::Millisecond> ||
						  std::is_same_v<T, Timer::Nanosecond>);

			if constexpr (std::is_same_v<T, Timer::Second>)
			{
				return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(mNowTime - mStartTime).count()) / 1e9f;
			}
			else if constexpr (std::is_same_v<T, Timer::Millisecond>)
			{
				return std::chrono::duration_cast<std::chrono::milliseconds>(mNowTime - mStartTime).count();
			}
			else if constexpr (std::is_same_v<T, Timer::Nanosecond>)
			{
				return std::chrono::duration_cast<std::chrono::nanoseconds>(mNowTime - mStartTime).count();
			}
		}

		template<typename T>
		typename T::Type Count()
		{
			mNowTime = std::chrono::high_resolution_clock::now();
			typename T::Type deltaTime = GetDeltaTime<T>();
			mLastFrameTime = mNowTime;
			return deltaTime;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
		std::chrono::time_point<std::chrono::high_resolution_clock> mNowTime;

	};
}