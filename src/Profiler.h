#ifndef PROFILER_H
#define PROFILER_H

#include <chrono>

typedef std::chrono::high_resolution_clock HighResClock;
typedef std::chrono::time_point<HighResClock> TimePoint;

class Profiler
{
private:
	const char* m_Name;
	TimePoint m_Start;

public:
	Profiler(const char* name)
		: m_Name(name)
	{
		m_Start = HighResClock::now();
	}

	~Profiler()
	{
		double duration = (HighResClock::now() - m_Start).count() / 1000000.0;
		//double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(HighResClock::now() - m_Start).count();
		printf("Profiler: %.3f ms %s", duration, m_Name);
	}
};

#define PROFILE_SCOPE(name) ::Profiler timer##__LINE__(name)

#endif // !PROFILER_H