#pragma region Includes 
#include <stdio.h> 
#include <windows.h> 
#include "ServiceInstaller.h" 
#include "ServiceBase.h" 
#include "SampleService.h" 
#include <strsafe.h>
#include <iostream>
#include <string.h>

#pragma endregion 


// Internal name of the service
#define SERVICE_NAME             TEXT("ServiceEvent") 

// Displayed name of the service 
#define SERVICE_DISPLAY_NAME     L"CppWindowsService Sample Service" 

// Service start options. 
//choices:
//SERVICE_AUTO_START ->A service started automatically by the service control manager during system startup
//SERVICE_DEMAND_START->A service started by the service control manager when a process calls the StartService
#define SERVICE_START_TYPE      SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0" 
#define SERVICE_DEPENDENCIES     L"" 

// The name of the account under which the service should run 
#define SERVICE_ACCOUNT          L"" 

// The password to the service account name 
#define SERVICE_PASSWORD         NULL

int wmain(int argc, wchar_t* argv[])
{
	if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
	{
		if (_wcsicmp(L"install", argv[1] + 1) == 0)
		{
			// Install the service when the command is  
			// "-install" or "/install". 
			InstallService(
				(PWSTR)SERVICE_NAME,               // Name of service 
				(PWSTR)SERVICE_NAME,       // Name to display 
				SERVICE_START_TYPE,         // Service start type 
				NULL,       // Dependencies 
				NULL,            // Service running account 
				NULL            // Password of the account 
			);
		}
		else if (_wcsicmp(L"remove", argv[1] + 1) == 0)
		{
			// Uninstall the service when the command is  
			// "-remove" or "/remove". 
			UninstallService((PWSTR)SERVICE_NAME);
		}
	}
	else
	{
		wprintf(L"Parameters:\n");
		wprintf(L" -install  to install the service.\n");
		wprintf(L" -remove   to remove the service.\n");

		CSampleService service((PWSTR)SERVICE_NAME);
		if (!CServiceBase::Run(service))
		{
			wprintf(L"Service failed to run w/err 0x%08lx\n", GetLastError());
		}
	}
	return 0;
}
