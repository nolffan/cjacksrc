/*
 * cjacksrc_debug.h -- diagnostics for the debug build, and nothing in release.
 *
 * The release build has to compile to the retail bytes, so nothing here may
 * reach it: without CJACK_DEBUG (which only the debug build defines, see
 * build/variant.sh) every macro below expands to nothing and this header
 * includes nothing. Arguments are not evaluated in release either, so never
 * put a side effect in them.
 *
 * Any file of any target can include it: src/cjacksrc/ is on every
 * compiler's include path (build/env.sh).
 *
 *   #include "cjacksrc_debug.h"
 *
 *   CJACK_LOG(("loading world %s, %d objects", pName, nObjects));
 *   CJACK_ASSERT(pObj != NULL);
 *
 *   #ifdef CJACK_DEBUG
 *       ... any other debug-only code ...
 *   #endif
 *
 * CJACK_LOG takes printf arguments in a second pair of parentheses (neither
 * VC .NET 2003 nor VC6 has variadic macros). Each line is written, with the
 * time in milliseconds and the source file and line, to cjacksrc.log in the
 * current directory (the game directory; $CJACKSRC_LOG names another file)
 * and to
 * OutputDebugString (shown by Wine with WINEDEBUG=+debugstr). CJACK_ASSERT
 * logs a failed condition and carries on.
 *
 * Plain C and C++, MFC or not: it declares the two Win32 functions it calls
 * rather than including <windows.h>, which MFC files must not see before
 * their own headers.
 */
#ifndef CJACK_DEBUG_H
#define CJACK_DEBUG_H

#ifdef CJACK_DEBUG

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) void __stdcall OutputDebugStringA(const char *lpOutputString);
__declspec(dllimport) unsigned long __stdcall GetTickCount(void);
#ifdef __cplusplus
}
#endif

/* where the next CjacksrcLogf line comes from (set by CJACK_LOG) */
static const char *g_CjacksrcLogFile;
static int g_CjacksrcLogLine;

static void CjacksrcLogAt(const char *file, int line)
{
    g_CjacksrcLogFile = file;
    g_CjacksrcLogLine = line;
}

static void __cdecl CjacksrcLogf(const char *fmt, ...)
{
    char buf[1024];
    int n;
    va_list ap;
    FILE *f;
    const char *path;

    if (g_CjacksrcLogLine > 0)
        n = _snprintf(buf, sizeof(buf) - 2, "[%8lu] %s(%d): ", GetTickCount(),
                      g_CjacksrcLogFile ? g_CjacksrcLogFile : "?", g_CjacksrcLogLine);
    else            /* no source position: just a tag */
        n = _snprintf(buf, sizeof(buf) - 2, "[%8lu] %s: ", GetTickCount(),
                      g_CjacksrcLogFile ? g_CjacksrcLogFile : "?");
    if (n < 0)
        n = 0;
    va_start(ap, fmt);
    if (_vsnprintf(buf + n, sizeof(buf) - 2 - n, fmt, ap) < 0)
        n = sizeof(buf) - 2;            /* truncated: keep what fits */
    else
        n += (int)strlen(buf + n);
    va_end(ap);
    buf[n++] = '\n';
    buf[n] = '\0';

    OutputDebugStringA(buf);
    path = getenv("CJACKSRC_LOG");
    f = fopen(path && *path ? path : "cjacksrc.log", "a");
    if (f) {
        fputs(buf, f);
        fclose(f);
    }
}

#define CJACK_LOG(args) (CjacksrcLogAt(__FILE__, __LINE__), CjacksrcLogf args)
#define CJACK_ASSERT(expr) \
    ((expr) ? (void)0 : (CjacksrcLogAt(__FILE__, __LINE__), \
                         CjacksrcLogf("assertion failed: %s", #expr)))

#else /* release: nothing */

#define CJACK_LOG(args) ((void)0)
#define CJACK_ASSERT(expr) ((void)0)

#endif /* CJACK_DEBUG */

#endif /* CJACK_DEBUG_H */
