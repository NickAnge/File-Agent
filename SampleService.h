#pragma once
#include "ServiceBase.h" 

class CSampleService : public CServiceBase
{
public:
	CSampleService(PWSTR pszServiceName,
		BOOL fCanStop = TRUE,
		BOOL fCanShutdown = TRUE,
		BOOL fCanPauseContinue = FALSE);
	virtual ~CSampleService(void);

	virtual void OnStart(DWORD dwArgc, LPWSTR* pszArgv);
	virtual void OnStop();
private:
	BOOL m_fStopping;
	HANDLE m_hStoppedEvent;
};
