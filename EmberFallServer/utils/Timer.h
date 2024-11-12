#pragma once 

template<typename T>
concept TimeUnit = std::chrono::_Is_duration_v<T>;

class Timer {
	using clock = std::chrono::high_resolution_clock;
	using rep = double;
	using period = std::nano;
	using time_point = clock::time_point;
	using duration = std::chrono::duration<double, period>;

	//sceduled event �� �켱�Ǵ� ������ �ִ�. 
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
	/// �� �Լ����� bool ���� ��ȯ�ϴ� Callable �� �־��ֽñ� �ٶ��ϴ�. 
	/// �ش� Callable �� true �� ��ȯ�ϸ� �� ���� time_point �� �ٽ� Event �� ȣ��˴ϴ�. 
	/// �ش� Callable �� false �� ��ȯ�ϸ� �� ���� time_point �� Event �� �����˴ϴ�.
	/// </summary>
	/// <typeparam name="rep"></typeparam>
	/// <typeparam name="period"></typeparam>
	/// <param name="time">-�� ���� �̺�Ʈ�� �ߵ��ϱ� ���� �ʿ��� �ð��Դϴ�.</param>
	/// <param name="callBack">-�̺�Ʈ �ߵ� �� ȣ���� �Լ��Դϴ�.</param>
	template<typename rep, typename period>
	void AddEvent(std::chrono::duration<rep, period> time, std::function<bool()>&& callBack) {
		mEvents.emplace(clock::now() + time, std::chrono::duration_cast<std::chrono::nanoseconds>(time), std::move(callBack));
	}

	[[maybe_unused]]
	double SetTimeScale(double scale = 1.0);
	double GetTimeScale();
	uint64_t GetFrameCount();
	// �� �������� ���۵� ������ ȣ���� 
	void AdvanceTime();
	// Scene �� ���۵� �� ȣ�� 
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