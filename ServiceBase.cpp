#pragma region Includes 
#include "ServiceBase.h" 
#include <assert.h> 
#include <strsafe.h> 
#include <iostream>


#pragma endregion 
using namespace std;
#pragma endregion

#pragma region Static Members 
CServiceBase* CServiceBase::s_service = NULL;

BOOL CServiceBase::Run(CServiceBase& service)
{
	s_service = &service;

	//The Services that thsi process handles ,SERVICE_NAME->Service main function,for each function.
	SERVICE_TABLE_ENTRY serviceTable[] =
	{
		{ service.m_name, ServiceMain },
		{ NULL, NULL }
	};

	// Connects the main thread of a service process to the service control  
	// manager, which causes the thread to be the service control dispatcher  
	// thread for the calling process. This call returns when the service has  
	// stopped. The process should simply terminate when the call returns. 
	return StartServiceCtrlDispatcher(serviceTable);
}
void WINAPI CServiceBase::ServiceMain(DWORD dwArgc, LPWSTR* pszArgv)
{
	assert(s_service != NULL);

	// Register the handler function for the service 
	s_service->m_statusHandle = RegisterServiceCtrlHandler(
		s_service->m_name, ServiceCtrlHandler);
	if (s_service->m_statusHandle == NULL)
	{
		throw GetLastError();
	}

	// Start the service. 
	s_service->Start(dwArgc, pszArgv);

}
void WINAPI CServiceBase::ServiceCtrlHandler(DWORD dwCtrl)
{
	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
	{
		s_service->Stop();
		break;
	}
	default:
		break;
	}
}
CServiceBase::CServiceBase(PWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue)
{
	// Service name must be a valid string and cannot be NULL. 
	m_name = (PWSTR)((pszServiceName == NULL) ? L"" : pszServiceName);

	m_statusHandle = NULL;

	// The service runs in its own process. 
	m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	// The service is starting. 
	m_status.dwCurrentState = SERVICE_START_PENDING;

	// The accepted commands of the service. 
	DWORD dwControlsAccepted = 0;
	if (fCanStop)
		dwControlsAccepted |= SERVICE_ACCEPT_STOP;
	if (fCanShutdown)
		dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
	if (fCanPauseContinue)
		dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
	m_status.dwControlsAccepted = dwControlsAccepted;

	m_status.dwWin32ExitCode = NO_ERROR;
	m_status.dwServiceSpecificExitCode = 0;
	m_status.dwCheckPoint = 0;
	m_status.dwWaitHint = 0;
}

CServiceBase::~CServiceBase(void)
{
}
void CServiceBase::Start(DWORD dwArgc, PWSTR* pszArgv)
{
	try
	{
		// Tell SCM that the service is starting. 
		SetServiceStatus(SERVICE_START_PENDING);

		// Perform service-specific initialization. 
		OnStart(dwArgc, pszArgv);
	}
	catch (DWORD dwError)
	{
		// Log the error. 
		WriteErrorLogEntry((PWSTR)L"Service Start", dwError);

		// Set the service status to be stopped. 
		SetServiceStatus(SERVICE_STOPPED, dwError);
	}
	catch (...)
	{
		// Log the error. 
		WriteEventLogEntry((PWSTR)L"Service failed to start.", EVENTLOG_ERROR_TYPE);

		// Set the service status to be stopped. 
		SetServiceStatus(SERVICE_STOPPED);
	}
}
void CServiceBase::OnStart(DWORD dwArgc, PWSTR* pszArgv)
{
}
void CServiceBase::Stop()
{
	DWORD dwOriginalState = m_status.dwCurrentState;
	try
	{
		// Tell SCM that the service is stopping. 
		SetServiceStatus(SERVICE_STOP_PENDING);

		// Perform service-specific stop operations. 
		OnStop();

		// Tell SCM that the service is stopped. 
		SetServiceStatus(SERVICE_STOPPED);
	}
	catch (DWORD dwError)
	{
		// Log the error. 
		WriteErrorLogEntry((PWSTR)L"Service Stop", dwError);

		// Set the orginal service status. 
		SetServiceStatus(dwOriginalState);
	}
	catch (...)
	{
		// Log the error. 
		WriteEventLogEntry((PWSTR)L"Service failed to stop.", EVENTLOG_ERROR_TYPE);

		// Set the orginal service status. 
		SetServiceStatus(dwOriginalState);
	}
}
void CServiceBase::OnStop()
{
}
void CServiceBase::SetServiceStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure of the service. 

	m_status.dwCurrentState = dwCurrentState;
	m_status.dwWin32ExitCode = dwWin32ExitCode;
	m_status.dwWaitHint = dwWaitHint;

	m_status.dwCheckPoint =
		((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED)) ?
		0 : dwCheckPoint++;

	// Report the status of the service to the SCM. 
	::SetServiceStatus(m_statusHandle, &m_status);
}
void CServiceBase::WriteEventLogEntry(PWSTR pszMessage, WORD wType)
{
	HANDLE hEventSource = NULL;
	LPCWSTR lpszStrings[2] = { NULL, NULL };

	hEventSource = RegisterEventSource(NULL, m_name);
	if (hEventSource)
	{
		lpszStrings[0] = m_name;
		lpszStrings[1] = pszMessage;

		ReportEvent(hEventSource,  // Event log handle 
			wType,                 // Event type 
			0,                     // Event category 
			0,                     // Event identifier 
			NULL,                  // No security identifier 
			2,                     // Size of lpszStrings array 
			0,                     // No binary data 
			lpszStrings,           // Array of strings 
			NULL                   // No binary data 
		);

		DeregisterEventSource(hEventSource);
	}
}
void CServiceBase::WriteErrorLogEntry(PWSTR pszFunction, DWORD dwError)
{
	wchar_t szMessage[260];
	StringCchPrintf(szMessage, ARRAYSIZE(szMessage),
		L"%s failed w/err 0x%08lx", pszFunction, dwError);
	WriteEventLogEntry(szMessage, EVENTLOG_ERROR_TYPE);
}

