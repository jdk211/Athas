#include "StdAfx.h"
#include "cTime_Manager.h"


cTime_Manager::cTime_Manager(void)
	: m_dwFrameRate(0)
	, m_dwFPSCount(0)
	, m_fFPSElapsed(0.f)
{
	m_dwLastUpdateTime = GetTickCount();
}


cTime_Manager::~cTime_Manager(void)
{
}

void cTime_Manager::Update()
{
	DWORD dwCurrTime = GetTickCount();
	m_dwDeltaTime = dwCurrTime - m_dwLastUpdateTime;
	m_dwLastUpdateTime = dwCurrTime;

	m_dwFPSCount++;
	m_fFPSElapsed += m_dwDeltaTime;

	if (m_fFPSElapsed > 1000.f)
	{
		m_dwFrameRate = m_dwFPSCount;
		m_dwFPSCount = 0;
		m_fFPSElapsed = 0.f;
	}
}

float cTime_Manager::GetDeltaTime()
{
	return m_dwDeltaTime / 1000.f;
}

DWORD cTime_Manager::GetFPS()
{
	return m_dwFrameRate;
}