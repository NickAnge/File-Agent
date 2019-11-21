#pragma once
#include <windows.h> 

class CServiceBase
{
public:
	// Register the executable for a service with the Service Control Manager  
	// (SCM). After you call Run(ServiceBase), the SCM issues a Start command,  
	// which results in a call to the OnStart method in the service. This  
	// method blocks until the service has stopped. 
	static BOOL Run(CServiceBase& service);

	// Service object constructor. The optional parameters (fCanStop,  
	// fCanShutdown and fCanPauseContinue) allow you to specify whether the  
	// service can be stopped, paused and continued, or be notified when  
	// system shutdown occurs. 
	CServiceBase(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE);

	// Service object destructor.  
	virtual ~CServiceBase(void);
	//SERVICE MAIN FUNCTIONALITY
	void WatchDirectory(LPTSTR lpDir);
	void RefreshDirectory(LPTSTR lpDir);
	void RefreshTree(LPTSTR lpDrive);
	void RefreshFile(LPTSTR lpDrive);
	// Stop the service. 
	void Stop();

protected:
	// When implemented in a derived class, executes when a Start command is  
	// sent to the service by the SCM or when the operating system starts  
	// (for a service that starts automatically). Specifies actions to take  
	// when the service starts. 

	virtual void OnStart(DWORD dwArgc, PWSTR* pszArgv);

	// When implemented in a derived class, executes when a Stop command is  
	// sent to the service by the SCM. Specifies actions to take when a  
	// service stops running. 
	virtual void OnStop();

	// Set the service status and report the status to the SCM. 
	void SetServiceStatus(DWORD dwCurrentState,
		DWORD dwWin32ExitCode = NO_ERROR,
		DWORD dwWaitHint = 0);

	// Log a message to the Application event log. 
	void WriteEventLogEntry(PWSTR pszMessage, WORD wType);

	// Log an error message to the Application event log. 
	void WriteErrorLogEntry(PWSTR pszFunction,
		DWORD dwError = GetLastError());

private:
	// Entry point for the service. It registers the handler function for the  
	// service and starts the service. 
	static void WINAPI ServiceMain(DWORD dwArgc, LPWSTR* lpszArgv);

	// The function is called by the SCM whenever a control code is sent to  
	// the service. 
	static void WINAPI ServiceCtrlHandler(DWORD dwCtrl);

	// Start the service. 
	void Start(DWORD dwArgc, PWSTR* pszArgv);

	// The singleton service instance. 
	static CServiceBase* s_service;

	// The name of the service 
	PWSTR m_name;

	// The status of the service 
	SERVICE_STATUS m_status;

	// The service status handle 
	SERVICE_STATUS_HANDLE m_statusHandle;
};
