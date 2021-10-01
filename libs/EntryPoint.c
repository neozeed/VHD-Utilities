#include <windows.h>

UINT __fastcall EntryMain( UINT argc, PTSTR *argv );

#ifdef UNICODE
#define GetArgv(pcArgs) CommandLineToArgvW(GetCommandLineW(), pcArgs)
#else
// GetArgv.c needs to be compiled and linked if Unicode is not used.
PSTR * __fastcall GetArgvA( INT *pcArgs );
#define GetArgv GetArgvA
#endif

#pragma comment(linker, "/entry:EntryPoint")
void EntryPoint( )
{
	UINT uRet = ~0;
	UINT argc = 0;
	PTSTR *argv = GetArgv(&argc);

	if (argv)
	{
		uRet = EntryMain(argc, argv);
		LocalFree(argv);
	}

	ExitProcess(uRet);
}
