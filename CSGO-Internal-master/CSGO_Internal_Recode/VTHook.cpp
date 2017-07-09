#include "VTHook.h"

CHook::CHook(void* pInstance)
{
	m_pInstance = nullptr;
	m_pCustomVmt = nullptr;
	m_pBackVmt = nullptr;
	m_nSize = -1;

	if(pInstance)
	{
		m_pInstance = (uintptr_t**)pInstance;
		m_pBackVmt = *m_pInstance;

		MEMORY_BASIC_INFORMATION mem = { 0 };

		do
		{
			m_nSize++;

			VirtualQuery((LPCVOID)m_pBackVmt[m_nSize], &mem, sizeof(MEMORY_BASIC_INFORMATION));
		} while(mem.Protect == PAGE_EXECUTE_READ || mem.Protect == PAGE_EXECUTE_READWRITE);

		if(m_nSize)
		{
			m_pCustomVmt = new uintptr_t[m_nSize];

			memcpy(m_pCustomVmt, m_pBackVmt, sizeof(uintptr_t)* m_nSize);

			*m_pInstance = m_pCustomVmt;
		}
	}
}

CHook::~CHook(void)
{
	if(!m_pCustomVmt || !m_pBackVmt || !*m_pInstance)
		return;

	if((uintptr_t)(*m_pInstance) == (uintptr_t)(m_pCustomVmt))
	{
		*m_pInstance = m_pBackVmt;

		delete m_pCustomVmt;

		m_pInstance = nullptr;
		m_pCustomVmt = nullptr;
		m_pBackVmt = nullptr;
		m_nSize = -1;
	}
}

void CHook::HookMethod(void* pHooked, size_t nIndex)
{
	m_pCustomVmt[nIndex] = (DWORD_PTR)pHooked;
}

void CHook::Unhook()
{
	*m_pInstance = m_pBackVmt;
}

void CHook::Rehook()
{
	*m_pInstance = m_pCustomVmt;
}