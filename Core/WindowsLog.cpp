#ifdef _WINDOWS
namespace Hawl{
void _FailedAssert(const char* file, int line, const char* statement)
{
	static bool debug = true;

	if (debug)
	{
		WCHAR str[1024];
		WCHAR message[1024];
		WCHAR wfile[1024];
		mbstowcs(message, statement, 1024);
		mbstowcs(wfile, file, 1024);
		wsprintfW(str, L"Failed: (%s)\n\nFile: %s\nLine: %d\n\n", message, wfile, line);

		if (IsDebuggerPresent())
		{
			wcscat(str, L"Debug?");
			int res = MessageBoxW(NULL, str, L"Assert failed", MB_YESNOCANCEL | MB_ICONERROR);
			if (res == IDYES)
			{
#if _MSC_VER >= 1400
				__debugbreak();
#else
				_asm int 0x03;
#endif
			}
			else if (res == IDCANCEL)
			{
				debug = false;
			}
		}
		else
		{
#if HAWL_STACKTRACE_DUMP
			__debugbreak();
#else
			wcscat(str, L"Display more asserts?");
			if (MessageBoxW(NULL, str, L"Assert failed", MB_YESNO | MB_ICONERROR | MB_DEFBUTTON2) != IDYES)
			{
				debug = false;
			}
#endif
		}
	}
}
}
#endif
