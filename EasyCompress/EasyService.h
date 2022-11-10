#pragma once

#ifndef _UNICODE
#include <string>
using namespace std;
#define String string
#else
#include <xstring>
using namespace std;
#define String wstring
#endif

#include <WinSvc.h>

class CEasyService
{
private:
	CEasyService(void);
	~CEasyService(void);

public:
	BOOL Install(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplay, DWORD dwStartType);

	BOOL IsInstalled();

	void	 LogEvent(LPCTSTR pFormat, ...)
	{	
		TCHAR    szMsg[256];
		va_list		args;

		va_start(args, pFormat);
		vswprintf_s(szMsg, 256, pFormat, args);
		va_end(args);

		OutputDebugString(szMsg);
	};

	BOOL Running(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplay);

	static VOID WINAPI ServiceMain(DWORD dwArgc,LPTSTR* lpszArgv);
	static VOID WINAPI ServiceHandler(DWORD fdwControl);

	static CEasyService *getInstance()
	{
		static CEasyService easy;
		return &easy;
	}


private:
	SERVICE_TABLE_ENTRY m_ServiceTable[2];

	String	m_strServiceName;
	String	m_strServiceDisplay;
	String	m_strPath;

	SC_HANDLE		m_hManager;
	SC_HANDLE		m_hService;

	BOOL				m_bRunning;
	int					m_nError;
	

public:
	SERVICE_STATUS_HANDLE	m_hStatus;
	SERVICE_STATUS					m_StatusInfo;
	BOOL									m_bLoop;
};
