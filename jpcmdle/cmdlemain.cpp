#include <Windows.h>
#include <Shlwapi.h>
#include <string>
#pragma comment(lib, "Shlwapi.lib")

/*++

Source:   https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/
Archived: https://web.archive.org/web/20180811234026/https://blogs.msdn.microsoft.com/twistylittlepassagesallalike/2011/04/23/everyone-quotes-command-line-arguments-the-wrong-way/

Routine Description:
	This routine appends the given argument to a command line such
	that CommandLineToArgvW will return the argument string unchanged.
	Arguments in a command line should be separated by spaces; this
	function does not add these spaces.

Arguments:
	Argument - Supplies the argument to encode.
	CommandLine - Supplies the command line to which we append the encoded argument string.
	Force - Supplies an indication of whether we should quote
			the argument even if it does not contain any characters that would
			ordinarily require quoting.

--*/
void ArgvQuote(const std::wstring& Argument, std::wstring& CommandLine, bool Force = false) {
	// Unless we're told otherwise, don't quote unless we actually
	// need to do so --- hopefully avoid problems if programs won't
	// parse quotes properly

	// CHANGES: Changed from std::wstring to std::string because this is 
	//          for ansi programs that require the Japanese code page.

	if (!CommandLine.empty())
		CommandLine.append(L" ");

	if (!Force && !Argument.empty() && Argument.find_first_of(L" \t\n\v\"") == Argument.npos) {
		CommandLine.append(Argument);
	}
	else {
		CommandLine.push_back(L'"');

		for (auto It = Argument.begin(); ; ++It) {
			unsigned NumberBackslashes = 0;

			while (It != Argument.end() && *It == L'\\') {
				++It;
				++NumberBackslashes;
			}

			if (It == Argument.end()) {
				// Escape all backslashes, but let the terminating
				// double quotation mark we add below be interpreted
				// as a metacharacter.

				CommandLine.append(NumberBackslashes * 2, L'\\');
				break;
			}
			else if (*It == L'"') {
				// Escape all backslashes and the following
				// double quotation mark.

				CommandLine.append(NumberBackslashes * 2 + 1, L'\\');
				CommandLine.push_back(*It);
			}
			else {
				// Backslashes aren't special here.

				CommandLine.append(NumberBackslashes, L'\\');
				CommandLine.push_back(*It);
			}
		}

		CommandLine.push_back(L'"');
	}
}

int wmain(int argc, wchar_t** argv) {
	// We're launching jpcmd
	std::wstring exe = L"";
	std::wstring argsPrefix = L"";
	std::wstring args = L"";

	// Add any additional command line arguments to the system call and escape them
	for (int i = 1; i < argc; i++) {
		ArgvQuote(argv[i], args, false);
	}

	wchar_t wcLEProc[MAX_PATH] = { 0 };
	wchar_t wcCurrentDir[MAX_PATH] = { 0 };
	wchar_t wcPath[MAX_PATH] = { 0 };
	wchar_t wcConfig[MAX_PATH] = { 0 };
	wchar_t wcJpcmd[MAX_PATH] = { 0 };

	GetModuleFileNameW(nullptr, wcPath, MAX_PATH);
	PathRemoveFileSpecW(wcPath);
	PathCombineW(wcConfig, wcPath, L"config.ini");
	PathCombineW(wcJpcmd, wcPath, L"jpcmd.exe");

	GetCurrentDirectoryW(MAX_PATH, wcCurrentDir);

	GetPrivateProfileStringW(L"Launch", L"LEProc", L"", wcLEProc, MAX_PATH, wcConfig);

	STARTUPINFOW StartupInfo = { 0 };
	PROCESS_INFORMATION ProcessInfo = { 0 };
	StartupInfo.cb = sizeof(STARTUPINFOW);

	if (wcLEProc[0] != L'\0') {
		//WritePrivateProfileStringW(L"Launch", L"Tmp_WorkingDir", wcCurrentDir, wcConfig);

		// We're relaunching jpcmd with full JP locale
		exe = wcLEProc;
		ArgvQuote(wcLEProc, argsPrefix, false);
		ArgvQuote(wcJpcmd, argsPrefix, false);
		if (args.empty())
			args = argsPrefix;
		else
			args = argsPrefix + L" " + args;

		// Extra measures to prevent a blank console window popping up before jpcmd launches
		StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
		StartupInfo.wShowWindow = SW_HIDE;
	}
	else {
		//WritePrivateProfileStringW(L"Launch", L"Tmp_WorkingDir", L"", wcConfig);
		exe = wcJpcmd;
	}

	// command line arguments cannot be a constant string for some ungodly reason
	wchar_t* wcArgs = new wchar_t[args.length() + 1];
	lstrcpyW(wcArgs, args.c_str());

	if (!CreateProcessW(
		exe.c_str(),
		wcArgs,
		nullptr,
		nullptr,
		FALSE,
		CREATE_NEW_CONSOLE,
		nullptr,
		wcCurrentDir,
		&StartupInfo,
		&ProcessInfo))
	{
		return GetLastError();
	}
	delete[] wcArgs;
}

int _stdcall WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	return wmain(__argc, __wargv);
}
