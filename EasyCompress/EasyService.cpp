#include "StdAfx.h"
#include "EasyService.h"

CEasyService::CEasyService(void)
{
	memset(&m_ServiceTable, 0, sizeof(m_ServiceTable));

	m_hService = NULL;
	m_hManager = NULL;
	m_bRunning = FALSE;
	m_hStatus = NULL;

	memset(&m_StatusInfo, 0, sizeof(m_StatusInfo));

	m_StatusInfo.dwServiceType = SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS;  
	m_StatusInfo.dwCurrentState = SERVICE_START_PENDING;  
	m_StatusInfo.dwControlsAccepted = SERVICE_ACCEPT_STOP;  
	m_StatusInfo.dwWin32ExitCode = 0;  
	m_StatusInfo.dwServiceSpecificExitCode = 0;  
	m_StatusInfo.dwCheckPoint = 0;  
	m_StatusInfo.dwWaitHint = 0;  

	m_bLoop = TRUE;
}

CEasyService::~CEasyService(void)
{
	if (NULL != m_hManager) {
		CloseServiceHandle(m_hManager);
	}

	if (NULL != m_hService) {
		CloseServiceHandle(m_hService);
	}
}

BOOL CEasyService::Install(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplay, DWORD dwStartType)
{
	TCHAR	szPath[MAX_PATH + 1] = {0};

	m_strServiceName = lpszServiceName;
	m_strServiceDisplay = lpszServiceDisplay;

	m_ServiceTable[0].lpServiceName = (LPWSTR)m_strServiceName.c_str();
	m_ServiceTable[0].lpServiceProc = ServiceMain;

	m_ServiceTable[1].lpServiceName = NULL;
	m_ServiceTable[1].lpServiceProc = NULL;

	if (!IsInstalled()) 
	{
		if (0 == GetModuleFileName(NULL, szPath, MAX_PATH)) {
			return FALSE;
		}

		m_strPath = szPath;

		m_hService = CreateService(m_hManager, m_strServiceName.c_str(), m_strServiceDisplay.c_str(), SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS, dwStartType, SERVICE_ERROR_NORMAL,
			m_strPath.c_str(), NULL, NULL, NULL, NULL, NULL);
	} 

	return NULL != m_hService;
}


BOOL CEasyService::Running(LPCTSTR lpszServiceName, LPCTSTR lpszServiceDisplay)
{
	m_strServiceName = lpszServiceName;
	m_strServiceDisplay = lpszServiceDisplay;

	m_ServiceTable[0].lpServiceName = (LPWSTR)m_strServiceName.c_str();
	m_ServiceTable[0].lpServiceProc = ServiceMain;

	m_ServiceTable[1].lpServiceName = NULL;
	m_ServiceTable[1].lpServiceProc = NULL;

	if (!IsInstalled()) {
		return FALSE;
	}

	if (NULL != m_hService){
		m_bRunning = StartServiceCtrlDispatcher(&m_ServiceTable[0]);
	}

	return m_bRunning;
}


BOOL CEasyService::IsInstalled()
{
	m_hManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (NULL == m_hManager) 
	{
		return TRUE;
	}

	m_hService = OpenService(m_hManager, m_strServiceName.c_str(), SERVICE_ALL_ACCESS);

	if (NULL == m_hService && ERROR_SERVICE_DOES_NOT_EXIST == GetLastError()) {
		return FALSE;
	}
	
	return TRUE;
}


VOID WINAPI CEasyService::ServiceHandler(DWORD fdwControl	)
{
	CEasyService *easySrv = CEasyService::getInstance();

	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:

		easySrv->m_StatusInfo.dwCurrentState = SERVICE_STOP_PENDING;
		SetServiceStatus(easySrv->m_hStatus, &easySrv->m_StatusInfo);

		easySrv->m_bLoop = FALSE;

		break;
	case SERVICE_CONTROL_PAUSE:
		break;
	case SERVICE_CONTROL_CONTINUE:
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	case SERVICE_CONTROL_SHUTDOWN:
		break;
	default:
		break;
	}
}
 
VOID WINAPI CEasyService::ServiceMain(DWORD dwArgc,LPTSTR* lpszArgv)
{
	CEasyService *easySrv = CEasyService::getInstance();

	easySrv->m_StatusInfo.dwCurrentState = SERVICE_START_PENDING;
	easySrv->m_StatusInfo.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	easySrv->m_hStatus = RegisterServiceCtrlHandler(easySrv->m_strServiceName.c_str(), ServiceHandler);

	if (easySrv->m_hStatus == NULL) {
		return;
	}

	SetServiceStatus(easySrv->m_hStatus, &easySrv->m_StatusInfo);

	easySrv->m_StatusInfo.dwWin32ExitCode = S_OK;
	easySrv->m_StatusInfo.dwCheckPoint = 0;
	easySrv->m_StatusInfo.dwWaitHint = 0;
	easySrv->m_StatusInfo.dwCurrentState = SERVICE_RUNNING;

	SetServiceStatus(easySrv->m_hStatus, &easySrv->m_StatusInfo);

	easySrv->LogEvent(_T("ok"));

	while(easySrv->m_bLoop) 
	{
		//在这里启动注册dll
		Sleep(1000);
		break;
	} 

	easySrv->m_StatusInfo.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(easySrv->m_hStatus, &easySrv->m_StatusInfo);
	return;
}