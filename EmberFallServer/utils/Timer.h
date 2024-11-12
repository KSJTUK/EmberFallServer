#pragma once 

template<typename T>
concept TimeUnit = std::chrono::_Is_duration_v<T>;

class Timer {
	using clock = std::chrono::high_resolution_clock;
	using rep = double;
	using period = std::nano;
	using time_point = clock::time_point;
	using duration = std::chrono::duration<double, period>;

	//sceduled event 가 우선되는 문제가 있다. 
	struct Event {
		Event(std::chrono::time_point<clock> time, std::chrono::nanoseconds timeout, std::function<bool()>&& callBack) {
			mTimeout = timeout;
			mInvokeTime = time;
			mCallBack = callBack;
		}

		Event(const Event& rhs) = default;
		Event(Event&& rhs) noexcept = default;
		Event& operator=(const Event& rhs) = default;
		Event& operator=(Event&& rhs) noexcept = default;

		bool operator<(const Event& rhs) const {
			return mInvokeTime < rhs.mInvokeTime;
		}

		std::chrono::time_point<clock> mInvokeTime{};
		std::chrono::nanoseconds mTimeout{};
		std::function<bool()> mCallBack{ []() {return false; } };
	};

public:
	enum class scaled {
		result_time_scaled,
		result_time_unscaled,
	};
public:
	Timer();
	~Timer();

	template<typename ResultTy = double, TimeUnit Tu = std::chrono::seconds>
	[[nodiscard]]
	ResultTy GetDeltaTime(scaled sc = scaled::result_time_unscaled) {
		if (sc == scaled::result_time_scaled) {
			return std::chrono::duration_cast<std::chrono::duration<ResultTy, typename Tu::period>>(mDeltaTime * mTimeScale).count();
		}
		// scaled::result_time_unscaled
		return std::chrono::duration_cast<std::chrono::duration<ResultTy, typename Tu::period>>(mDeltaTime).count();
	}

	template<typename ResultTy = double, TimeUnit Tu = std::chrono::seconds>
	[[nodiscard]]
	ResultTy GetTimeSinceStarted(scaled sc = scaled::result_time_unscaled) {
		if (sc == scaled::result_time_scaled) {
			return std::chrono::duration_cast<std::chrono::duration<ResultTy, typename Tu::period>>(mScaledStarted).count();
		}
		// scaled::result_time_unscaled
		duration AbsoluteElapsed = clock::now() - mAbsoluteStarted;
		return std::chrono::duration_cast<std::chrono::duration<ResultTy, typename Tu::period>>(AbsoluteElapsed).count();
	}

	template<typename ResultTy = double, TimeUnit Tu = std::chrono::seconds>
	[[nodiscard]]
	ResultTy GetTimeSinceSceneStarted() {
		duration Elapsed = clock::now() - mSceneStarted;
		return std::chrono::duration_cast<std::chrono::duration<ResultTy, typename Tu::period>>(Elapsed).count();
	}

	template<typename ResultTy = double, TimeUnit Tu = std::chrono::seconds>
	[[nodiscard]]
	ResultTy GetSmoothDeltaTime() {
		auto sumofSamples = std::accumulate(mDeltaTimeBuffer.begin(), mDeltaTimeBuffer.end(), duration::zero(),
			[](const duration& a, const duration& b) {
				if (b.count() <= 0.0)
					return a;
				return a + b;
			});
		return std::chrono::duration_cast<std::chrono::duration<ResultTy, typename Tu::period>>(sumofSamples / mDeltaTimeBufferSize).count();
	}
	/// <summary>
	/// 이 함수에는 bool 형을 반환하는 Callable 을 넣어주시기 바랍니다. 
	/// 해당 Callable 이 true 를 반환하면 그 다음 time_point 에 다시 Event 가 호출됩니다. 
	/// 해당 Callable 이 false 를 반환하면 그 다음 time_point 에 Event 가 삭제됩니다.
	/// </summary>
	/// <typeparam name="rep"></typeparam>
	/// <typeparam name="period"></typeparam>
	/// <param name="time">-그 다음 이벤트가 발동하기 까지 필요한 시간입니다.</param>
	/// <param name="callBack">-이벤트 발동 시 호출할 함수입니다.</param>
	template<typename rep, typename period>
	void AddEvent(std::chrono::duration<rep, period> time, std::function<bool()>&& callBack) {
		mEvents.emplace(clock::now() + time, std::chrono::duration_cast<std::chrono::nanoseconds>(time), std::move(callBack));
	}

	[[maybe_unused]]
	double SetTimeScale(double scale = 1.0);
	double GetTimeScale();
	uint64_t GetFrameCount();
	// 각 프레임이 시작될 때마다 호출함 
	void AdvanceTime();
	// Scene 이 시작될 때 호출 
	void StartSceneTime();
private:
	void UpdateDeltaTime();
	void AddScaledStarted();
	void SampleDeltaTime();
	bool PopEvent();
	void CheckEvent();
private:
	duration				mDeltaTime{};
	duration				mScaledStarted{};

	uint64_t				mFrameCount{ 0 };

	static constexpr UINT mDeltaTimeBufferSize = 10;
	UINT mDeltaTimeSampleingIndex = 0;
	std::array<duration, mDeltaTimeBufferSize> mDeltaTimeBuffer{};


	time_point				mPrev{ clock::now() };
	time_point 				mSceneStarted{ clock::now() };
	const time_point		mAbsoluteStarted{ clock::now() };
	double					mTimeScale{ 1.0 };

	std::set<Event>			mEvents{};
};

extern class Timer gTimer;