#include <Windows.h>
//#include <Shlwapi.h>
#include <string>
//#pragma comment(lib, "Shlwapi.lib")

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
	// Constants
	const UINT SHIFT_JIS = 932; // Codepage
	const LANGID JP_LANG = MAKELANGID(LANG_JAPANESE, SUBLANG_JAPANESE_JAPAN);
	const LCID JP_LOCALE = MAKELCID(JP_LANG, SORT_JAPANESE_XJIS);
	LCID currentLocale = GetThreadLocale();

	//printf_s("%i\n", JP_LOCALE);

	// Set an appropriate window title
	if (currentLocale == JP_LOCALE)
		SetConsoleTitleW(L"Japanese Command Prompt (LE)");
	else
		SetConsoleTitleW(L"Japanese Command Prompt");

	// We're launching cmd
	std::wstring args = L"cmd";

	// Add any additional command line arguments to the system call and escape them
	for (int i = 1; i < argc; i++) {
		ArgvQuote(argv[i], args, false);
	}

	// Convert the multi-byte command line arguments to the correct encoding
	// _wcstombs_s_l uses the current locale, so we can't change that yet yet.
	auto shiftJISLocale = _create_locale(LC_CTYPE, "Japanese");
	char* cArgs = new char[(args.length() + 20) * 2];
	size_t convertSize = 0;
	_wcstombs_s_l(&convertSize, cArgs, (size_t)((args.length() + 20) * 2),
		args.c_str(), _TRUNCATE, shiftJISLocale);

	// Set the output and input code page and langauge
	SetConsoleOutputCP(SHIFT_JIS);
	SetConsoleCP(SHIFT_JIS);
	SetThreadLocale(JP_LOCALE);
	//SetThreadUILanguage(JP_LANG);

	// If we're launching from Locale emulator, we'll need to revert the working directory
	/*if (currentLocale == JP_LOCALE) {
		wchar_t wcPath[MAX_PATH] = { 0 };
		wchar_t wcConfig[MAX_PATH] = { 0 };
		wchar_t wcCurrentDir[MAX_PATH] = { 0 };

		GetModuleFileNameW(nullptr, wcPath, MAX_PATH);
		PathRemoveFileSpecW(wcPath);
		PathCombineW(wcConfig, wcPath, L"config.ini");

		GetPrivateProfileStringW(L"Launch", L"Tmp_WorkingDir", L"", wcCurrentDir, MAX_PATH, wcConfig);
		WritePrivateProfileStringW(L"Launch", L"Tmp_WorkingDir", L"", wcConfig);

		if (wcCurrentDir[0] != L'\0') {
			//SetCurrentDirectoryW(wcCurrentDir);
		}
	}*/

	// Launch the command prompt
	//_wsystem(args.c_str());
	system(cArgs);
	delete[] cArgs;
}