#include <windows.h>

/**
 * ParseUnsignedInteger: string to uint64; base 10 or 16
 **/
BOOLEAN __fastcall ParseUnsignedInteger( PCTSTR pszIn, PUINT64 puiOut )
{
	BOOLEAN fHex = FALSE;

	PCTSTR pszHead = NULL, pszTail;
	TCHAR ch;

	*puiOut = 0;

	// Strip away leading and trailing whitespace

	while (ch = *pszIn)
	{
		// After this loop, pszHead will point to the first non-whitespace char
		// and pszTail will point to the final non-whitespace char; pszTail is
		// guaranteed to be valid if pszHead is non-NULL.

		if (ch > TEXT(' '))
		{
			if (!pszHead)
				pszHead = pszIn;

			pszTail = pszIn;
		}

		++pszIn;
	}

	if (!pszHead)
		return(FALSE);

	// Detect the "0x" and "h" affixes for hexadecimal input

	if (pszHead[0] == TEXT('0') && (pszHead[1] | 0x20) == TEXT('x'))
	{
		fHex = TRUE;
		pszHead += 2;
	}
	else if (*pszTail == TEXT('h'))
	{
		fHex = TRUE;
		--pszTail;
	}

	// Fail if, after stripping whitespace and affixes, the input is blank

	if (pszHead > pszTail)
		return(FALSE);

	// Convert

	while (pszHead <= pszTail)
	{
		ch = *pszHead;

		if (ch >= TEXT('0') && ch <= TEXT('9'))
		{
			*puiOut *= (fHex) ? 0x10 : 10;
			*puiOut += ch - TEXT('0');
		}
		else if (fHex)
		{
			ch |= 0x20;

			if (ch >= TEXT('a') && ch <= TEXT('f'))
			{
				*puiOut *= 0x10;
				*puiOut += ch - TEXT('a') + 10;
			}
			else
				return(FALSE); // Illegal character
		}
		else if (ch > TEXT(' ') && ch != TEXT(',') && ch != TEXT('.')) // Exempt delimiters, for decimal input only
			return(FALSE); // Illegal character

		++pszHead;
	}

	return(TRUE);
}
