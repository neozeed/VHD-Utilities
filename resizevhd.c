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
	UINT64 cbTargetSize = 0;
	BOOLEAN fForce = FALSE;
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
			else
				fShowUsage = TRUE;
		}
		else if (!fForce && CheckFlagI(argv[i], TEXT('f')))
			fForce = TRUE;
		else
			fShowUsage = TRUE;
	}

	// Parse the target size

	if (!pszTargetSize)
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
		_tprintf(TEXT("Usage: resizevhd [-f] vhd size\n\n"));
		_tprintf(TEXT("  -f      Forcibly truncate the data stored within the VHD, if necessary.\n")
		         TEXT("  vhd     Path of the VHD file to be resized.\n")
		         TEXT("  size    Maximum size, in bytes, of the VHD; must be a multiple of 512.\n")
		         TEXT("          Alternatively, DVD or BD can be specified for preset sizes.\n")
		         TEXT("          If set to 0, reduce the VHD to the smallest possible size.\n"));

		return(~1);
	}
	else
	{
		VIRTUAL_STORAGE_TYPE vst;

		HANDLE hVhd;
		DWORD dwResult;
		DWORD cchMessage;
		PTSTR pszErrorMessage = NULL;

		PTSTR pszTargetExt = _tcsrchr(pszTargetPath, TEXT('.'));

		vst.DeviceId = (pszTargetExt && lstrcmpi(pszTargetExt + 1, TEXT("vhdx")) == 0) ?
			VIRTUAL_STORAGE_TYPE_DEVICE_VHDX :
			VIRTUAL_STORAGE_TYPE_DEVICE_VHD;
		vst.VendorId = VIRTUAL_STORAGE_TYPE_VENDOR_MICROSOFT;

		dwResult = OpenVirtualDisk(
			&vst,
			pszTargetPath,
			VIRTUAL_DISK_ACCESS_ALL,
			OPEN_VIRTUAL_DISK_FLAG_NONE,
			NULL,
			&hVhd
		);

		if (dwResult == ERROR_SUCCESS)
		{
			RESIZE_VIRTUAL_DISK_PARAMETERS rvdp;
			rvdp.Version = RESIZE_VIRTUAL_DISK_VERSION_1;
			rvdp.Version1.NewSize = cbTargetSize;

			dwResult = ResizeVirtualDisk(
				hVhd,
				(rvdp.Version1.NewSize == 0) ?
					RESIZE_VIRTUAL_DISK_FLAG_RESIZE_TO_SMALLEST_SAFE_VIRTUAL_SIZE : (fForce) ?
					RESIZE_VIRTUAL_DISK_FLAG_ALLOW_UNSAFE_VIRTUAL_SIZE :
					RESIZE_VIRTUAL_DISK_FLAG_NONE,
				&rvdp,
				NULL
			);

			CloseHandle(hVhd);
		}

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
