#ifndef __PARSENUMBER_H__
#define __PARSENUMBER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

/**
 * ParseUnsignedInteger: string to uint64; base 10 or 16
 **/
BOOLEAN __fastcall ParseUnsignedInteger( PCTSTR pszIn, PUINT64 puiOut );

#ifdef __cplusplus
}
#endif

#endif
