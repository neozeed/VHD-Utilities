#pragma comment(linker, "/version:1.0") // MUST be in the form of major.minor

#define INITGUID

#include <windows.h>
#include <virtdisk.h>
#include <stdio.h>
#include <tchar.h>
#include "libs\StringIntrinsics.h"
#include "libs\ParseNumber.h"

__forceinline BOOLEAN IsFlag( PCTSTR pszArg );
__forceinline BOOLEAN CheckFlagI( PCTSTR pszArg, TCHAR ch );

UINT __fastcall EntryMain( UINT argc, PTSTR *argv )
{
	PTSTR pszTargetPath = NULL;
	PTSTR pszTargetSize = NULL;
	PTSTR pszSourcePath = NULL;
	UINT64 cbTargetSize = 0;
	BOOLEAN fDynamic = FALSE;
	BOOLEAN fDifferencing = FALSE;
	BOOLEAN fShowUsage = FALSE;

	// Parse command line parameters

	UINT i;

	for (i = 1; i < argc; ++i)
	{
		if (!IsFlag(argv[i]))
		{
			if (pszTargetPath == NULL)
				pszTargetPath = argv[i];
			else if (pszTargetSize == NULL)
				pszTargetSize = argv[i];
			else if (pszSourcePath == NULL)
				pszSourcePath = argv[i];
			else
				fShowUsage = TRUE;
		}
		else if (!fDynamic && CheckFlagI(argv[i], TEXT('d')))
			fDynamic = TRUE;
		else if (!fDifferencing && CheckFlagI(argv[i], TEXT('f')))
			fDifferencing = TRUE;
		else
			fShowUsage = TRUE;
	}

	if (fDifferencing)
	{
		// It is possible for the -f flag to be specified at the end, in which
		// case, the meaning of the second non-flag parameter can only be
		// determined after a complete pass of the parameters.

		if (pszTargetSize && !pszSourcePath)
			pszSourcePath = pszTargetSize;
		else
			fShowUsage = TRUE;
	}

	// Parse the target size

	else if (!pszTargetSize)
		fShowUsage = TRUE;
	else if (lstrcmpi(pszTargetSize, TEXT("DVD")) == 0)
		cbTargetSize = 0x118200000; // 4,699,717,632 B or 4,482 MiB
	else if (lstrcmpi(pszTargetSize, TEXT("BD")) == 0)
		cbTargetSize = 0x5D2200000; // 25,000,148,992 B or 23,842 MiB
	else if (!ParseUnsignedInteger(pszTargetSize, &cbTargetSize))
		fShowUsage = TRUE;

	// Act according to the command line parameters

	if (fShowUsage)
	{
		_tprintf(TEXT("Usage: makevhd [-d] vhd size [source]\n\n"));
		_tprintf(TEXT("  -d      Use dynamic allocation for the virtual disk.\n")
		         TEXT("  vhd     Path of the VHD file to be created.\n")
		         TEXT("  size    Maximum size, in bytes, of the VHD; must be a multiple of 512.\n")
		         TEXT("          Alternatively, DVD or BD can be specified for preset sizes.\n")
		         TEXT("          If set to 0, the size of the source disk, if available, will be used.\n")
		         TEXT("  source  Path of an optional source whose data will populate the new VHD.\n\n"));

		_tprintf(TEXT("Usage: makevhd -f vhd parent\n\n"));
		_tprintf(TEXT("  -f      Create a differencing VHD.\n")
		         TEXT("  vhd     Path of the differencing VHD file to be created.\n")
		         TEXT("  parent  Path of an existing disk to be associated as the parent.\n"));

		return(~1);
	}
	else
	{
		VIRTUAL_STORAGE_TYPE vst;
		CREATE_VIRTUAL_DISK_PARAMETERS cvdp;

		HANDLE hVhd;
		DWORD dwResult;
		DWORD cchMessage;
		PTSTR pszErrorMessage = NULL;

		PTSTR pszTargetExt = _tcsrchr(pszTargetPath, TEXT('.'));

		vst.DeviceId = (pszTargetExt && lstrcmpi(pszTargetExt + 1, TEXT("vhdx")) == 0) ?
			VIRTUAL_STORAGE_TYPE_DEVICE_VHDX :
			VIRTUAL_STORAGE_TYPE_DEVICE_VHD;
		vst.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;

		ZeroMemory(&cvdp, sizeof(cvdp));
		cvdp.Version = CREATE_VIRTUAL_DISK_VERSION_1;
		cvdp.Version1.MaximumSize = cbTargetSize;
		cvdp.Version1.SectorSizeInBytes = CREATE_VIRTUAL_DISK_PARAMETERS_DEFAULT_SECTOR_SIZE;

		if (fDifferencing)
			cvdp.Version1.ParentPath = pszSourcePath;
		else
			cvdp.Version1.SourcePath = pszSourcePath;

		dwResult = CreateVirtualDisk(
			&vst,
			pszTargetPath,
			VIRTUAL_DISK_ACCESS_CREATE,
			NULL,
			(fDynamic || fDifferencing) ? 0 : CREATE_VIRTUAL_DISK_FLAG_FULL_PHYSICAL_ALLOCATION,
			0,
			&cvdp,
			NULL,
			&hVhd
		);

		if (dwResult == ERROR_SUCCESS)
			CloseHandle(hVhd);

		cchMessage = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwResult,
			0,
			(PTSTR)&pszErrorMessage,
			0,
			NULL
		);

		if (cchMessage)
			_ftprintf(stderr, TEXT("%s"), pszErrorMessage);
		else
			_ftprintf(stderr, TEXT("Code %08X\n"), dwResult);

		LocalFree(pszErrorMessage);

		return(dwResult);
	}
}

__forceinline BOOLEAN IsFlag( PCTSTR pszArg )
{
	return(
		(pszArg[0] | 0x02) == TEXT('/') &&
		(pszArg[1]       ) != 0 &&
		(pszArg[2]       ) == 0
	);
}

__forceinline BOOLEAN CheckFlagI( PCTSTR pszArg, TCHAR ch )
{
	return(
		(pszArg[1] | 0x20) == ch
	);
}
