#pragma once

class CriticalSection
{
private:
	CRITICAL_SECTION m_Section;

public:
	CriticalSection()
	{
		InitializeCriticalSection(&m_Section);
	}

	~CriticalSection()
	{
		DeleteCriticalSection(&m_Section);
	}

	void Enter()
	{
		EnterCriticalSection(&m_Section);
	}

	void Leave()
	{
		LeaveCriticalSection(&m_Section);
	}

	CriticalSection(const CriticalSection&) = delete;
	CriticalSection& operator=(const CriticalSection&) = delete;

	class ScopedLock
	{
	private:
		CriticalSection& m_CriticalSection;

	public:
		ScopedLock(CriticalSection& criticalSection) :
			m_CriticalSection(criticalSection)
		{
			m_CriticalSection.Enter();
		}

		~ScopedLock()
		{
			m_CriticalSection.Leave();
		}
	};
};

