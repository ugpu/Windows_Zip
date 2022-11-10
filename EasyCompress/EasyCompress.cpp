// EasyCompress.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EasyService.h"



BOOL GetProcessUserName(HANDLE hProcess, String &strUserName)
{
	HANDLE				hToken = NULL;
	DWORD				dwUserSize = 0;
	BOOL					bResult = FALSE;

	TCHAR					userBuf[200] = {0};
	PTOKEN_USER		pUser = (PTOKEN_USER)userBuf;

	TCHAR					szUserName[121] = {0};
	TCHAR					szDomainName[121] = {0};

	DWORD				dwNameSize = 120;
	DWORD				dwDomainSize = 120;
	SID_NAME_USE	nameUser;

	if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken) || 
		NULL == hToken) {
		goto exit0;
	}

	if (GetTokenInformation(hToken, TokenUser, pUser, sizeof(userBuf), &dwUserSize)) 
	{
		bResult = LookupAccountSid(NULL, pUser->User.Sid, szUserName, &dwNameSize, szDomainName, &dwDomainSize, &nameUser);

		if (bResult) {
			strUserName = szUserName;
		}
	}

exit0:

	if (NULL != hToken) {
		CloseHandle(hToken);
	}
	return bResult;
}



int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	DWORD dwSid = 0;
	String strName;

	if (GetProcessUserName(GetCurrentProcess(), strName) && 
		(0 == strName.compare(_T("system")) ||
		0 == strName.compare(_T("SYSTEM"))))
	{
		CEasyService::getInstance()->LogEvent(_T("begin running\\n"));
		CEasyService::getInstance()->Running(_T("testabcd"), _T("谁点谁是逗比"));
	}
	else
	{
		CEasyService::getInstance()->LogEvent(_T("begin install\\n"));
		CEasyService::getInstance()->Install(_T("testabcd"), _T("谁点谁是逗比"), SERVICE_AUTO_START);
	}

 	return 0;
}