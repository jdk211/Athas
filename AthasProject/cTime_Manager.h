#pragma once

#define g_pTimeManager cTimeManager::GetInstance()

class cTime_Manager
{
private:
	SINGLETONE(cTime_Manager);
private:
	unsigned long m_dwLastUpdateTime;
	unsigned long m_dwDeltaTime;
	unsigned long m_dwFPSCount;
	unsigned long m_dwFrameRate;
	unsigned long m_fFPSElapsed;

public:
	void Update();
	float GetDeltaTime();
	unsigned long GetFPS();
};

