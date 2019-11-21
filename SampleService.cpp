#pragma region Includes 
#include "SampleService.h" 
#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <fstream>
#include <direct.h>

#pragma endregion
BOOL stopEvent = FALSE;
HANDLE dwChangeHandles[2];
CSampleService::CSampleService(PWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue)
	: CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue)
{
	m_fStopping = FALSE;

	// Create a manual-reset event that is not signaled at first to indicate  
	// the stopped signal of the service. 
	m_hStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hStoppedEvent == NULL)
	{
		throw GetLastError();
	}
}
CSampleService::~CSampleService(void)
{
	if (m_hStoppedEvent)
	{
		CloseHandle(m_hStoppedEvent);
		m_hStoppedEvent = NULL;
	}
}
void CSampleService::OnStart(DWORD dwArgc, LPWSTR* lpszArgv)
{
	// Log a service start message to the Application log. 
	WriteEventLogEntry((PWSTR)L"ServiceEvent in OnStart",
		EVENTLOG_INFORMATION_TYPE);
	//change SCM SERVICE status
	SetServiceStatus(SERVICE_RUNNING);

	//Function that checks the changes inside C directory
	//This function for start,takes paremeter the directory you want to check 
	CServiceBase::WatchDirectory(LPTSTR(TEXT("C:\\Games\\nikos")));

}
void CServiceBase::WatchDirectory(LPTSTR lpDir) {
	/*This Fucntion takes lpDir for parameter and create 3 handles for checking changes inside of it .
	First handle is for files and second handle is for directories and third for checking updates
	ALL INFORMATIONS ABOUT DIRECTORIES AND FILES(WHEN HANDLES FIRED) IS DISPLAYED ON 
	EVENT VIÅWER->WINDOW LOGS ->APPLICATION*/

	DWORD dwWaitStatus;
	HANDLE dwChangeHandles[3];
	TCHAR lpDrive[4];
	TCHAR lpFile[_MAX_FNAME];
	TCHAR lpExt[_MAX_EXT];

	_tsplitpath_s(lpDir, lpDrive, 4, NULL, 0, lpFile, _MAX_FNAME, lpExt, _MAX_EXT);
	lpDrive[2] = (TCHAR)'\\';
	lpDrive[3] = (TCHAR)'\0';

	//First handle for creating,deleting,renaming files
	dwChangeHandles[0] = FindFirstChangeNotification(
		lpDir,                         // directory to watch 
		FALSE,                         // do not watch subtree 
		FILE_NOTIFY_CHANGE_FILE_NAME); // watch file name changes 

	if (dwChangeHandles[0] == INVALID_HANDLE_VALUE)
	{
		WriteEventLogEntry((PWSTR)L"INVALID HANDLE VALUE",
			EVENTLOG_ERROR_TYPE);
		return;
	}
	//Second handle for creating,deleting,renaming directories
	dwChangeHandles[1] = FindFirstChangeNotification(
		lpDrive,                       // directory to watch 
		TRUE,                          // watch the subtree 
		FILE_NOTIFY_CHANGE_DIR_NAME);  // watch dir name changes 

	if (dwChangeHandles[1] == INVALID_HANDLE_VALUE)
	{
		WriteEventLogEntry((PWSTR)L"INVALID HANDLE VALUE",
			EVENTLOG_ERROR_TYPE);
		return;
	}
	if ((dwChangeHandles[0] == NULL) || (dwChangeHandles[1] == NULL))
	{
		WriteEventLogEntry((PWSTR)L"INVALID HANDLE VALUE",
			EVENTLOG_ERROR_TYPE);
		return;
	}
	dwChangeHandles[2] = FindFirstChangeNotification(
		lpDir,                       // directory to watch 
		FALSE,                          // watch the subtree 
		FILE_NOTIFY_CHANGE_LAST_WRITE);//last time writes
	if (dwChangeHandles[2] == INVALID_HANDLE_VALUE)
	{
		WriteEventLogEntry((PWSTR)L"INVALID HANDLE VALUE",
			EVENTLOG_ERROR_TYPE);
		return;
	}
	while (TRUE) {
		WriteEventLogEntry((PWSTR)L"Waiting Notifications.....",
			EVENTLOG_INFORMATION_TYPE);

		dwWaitStatus = WaitForMultipleObjects(3, dwChangeHandles,
			FALSE, INFINITE);
		switch (dwWaitStatus)
		{
		case WAIT_OBJECT_0 +0:
			// A file was created, renamed, or deleted in the directory.
			// Refresh this directory and restart the notification.
			RefreshDirectory(lpDir);
			if (FindNextChangeNotification(dwChangeHandles[0]) == FALSE)
			{
				ExitProcess(GetLastError());
			}
			break;
		case WAIT_OBJECT_0 +1:
			// A directory was created, renamed, or deleted.
			// Refresh the tree and restart the notification.
			RefreshTree(lpDir);
			if (FindNextChangeNotification(dwChangeHandles[1]) == FALSE)
			{
				ExitProcess(GetLastError());
			}
			break;
		case WAIT_OBJECT_0 +2:
			//A file has been updated
			//Print something and restart the notification
			RefreshFile(lpDir);
			if (FindNextChangeNotification(dwChangeHandles[2]) == FALSE)
			{
				ExitProcess(GetLastError());
			}
			break;
		case WAIT_TIMEOUT:
			// A timeout occurred, this would happen if some value other 
			// than INFINITE is used in the Wait call and no changes occur.
			break;
		default:
			ExitProcess(0);
			break;
		}
	}
}
void CServiceBase::RefreshDirectory(LPTSTR lpDir)
{
	WriteEventLogEntry((PWSTR)L"File changed.....",
		EVENTLOG_INFORMATION_TYPE);
}
void CServiceBase::RefreshTree(LPTSTR lpDrive) {
	WriteEventLogEntry((PWSTR)L"Directory changed.....",
		EVENTLOG_INFORMATION_TYPE);

}
void CServiceBase::RefreshFile(LPTSTR lpDrive) {
	WriteEventLogEntry((PWSTR)L"UPDATE happened.....",
		EVENTLOG_INFORMATION_TYPE);
}
void CSampleService::OnStop()
{
	// Log a service stop message to the Application log. 
	WriteEventLogEntry((PWSTR)L"ServiceEvent in OnStop",
		EVENTLOG_INFORMATION_TYPE);
}


