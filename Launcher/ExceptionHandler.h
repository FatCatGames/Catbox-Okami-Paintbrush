#include <Windows.h>
#include <DbgHelp.h>
#include <filesystem>
#include <time.h>

/*
	TODO: Read https://learn.microsoft.com/en-us/windows/win32/dxtecharts/crash-dump-analysis

	If program crashes outside of Visual Studio create popup with the following text:
	”Program crashed! A minidump was created at [PATH]/MiniDump.dmp, please tell a programmer.”

	Include dbghelp.h and add corresponding .lib file
*/

//void RunGame();
void CreateMiniDump(EXCEPTION_POINTERS* someExceptionPointers);
LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aExceptionP);

// This is how the exception Handler works, put the code in the try block and it will catch exceptions.
//int main(int argc, char* argv[])
//{
//	__try
//	{
//        int* pBadPtr = NULL;
//        *pBadPtr = 0;
//		  RunGame(); 
//	}
//	__except (ExceptionFilterFunction(GetExceptionInformation()))
//	{
//
//	}
//}

std::string GetTime()
{

	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	localtime_s(&tstruct, &now);
	strftime(buf, 80, "%F %H_%M", &tstruct);

	return buf;
}

void CreateMiniDump(EXCEPTION_POINTERS* someExceptionPointers)
{
	std::string path = "Temp\\ProggMoment " + GetTime() + ".dmp";

	std::filesystem::path miniDumpPath = path;

	std::wstring wstr = L"Odjå, någonting har gått snett. (Fungerar faktiskt på min dator): \n\n";
	wstr.append(std::filesystem::current_path());
	wstr.append(L"\\");
	wstr.append(path.begin(), path.end());
	wstr.append(L"\n\nGlöm inte skicka till dina progg.");

	HANDLE hFile = CreateFile(miniDumpPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, wstr.c_str(), L"Error", MB_OK | MB_SYSTEMMODAL);
		MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
		exceptionInfo.ThreadId = GetCurrentThreadId();
		exceptionInfo.ExceptionPointers = someExceptionPointers;
		exceptionInfo.ClientPointers = TRUE;

		// Write the mini dump
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &exceptionInfo, NULL, NULL);

		CloseHandle(hFile);
	}
}

LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aExceptionP)
{
	CreateMiniDump(aExceptionP);

	return EXCEPTION_EXECUTE_HANDLER;
}