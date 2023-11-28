/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// CommonStuff.cpp - Miscelaneous code that doesn't belong elsewhere.
//
// Visit gamemath.com for the latest version of this file.
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <MyEngine/Common.h>

MYENGINE_NS_BEGIN

const char* gAbortSourceFile = "(unknown)";
int gAbortSourceLine;

/////////////////////////////////////////////////////////////////////////////
//
// global code
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// areWeBeingDebugged()
//
// Return TRUE if we are running under the debugger

BOOL IsDebugged() {
#ifdef WIN32
    return IsDebuggerPresent();
#else
    // For now, let's assume we are being debugged
    // in a debug build, and otherwise we're not being
    // debugged
#ifdef _DEBUG
    return TRUE;
#else
    return FALSE;
#endif
#endif
}

//---------------------------------------------------------------------------
// reallyAbort
//
// Fatal error.  Usually called through the ABORT macro

void DoAbort(const char* fmt, ...) {
    // Format the error message into our buffer

    char errMsg[1024];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(errMsg, fmt, ap);
    va_end(ap);

    // Tack on the source file and line number

    sprintf(strchr(errMsg, '\0'), "\n%s line %d", gAbortSourceFile, gAbortSourceLine);

    // Windows?  Dump message box

#ifdef WIN32

    // Check if we're under the debugger

    if (IsDebugged()) {
        // Dump error message to the debug console

        OutputDebugString("FATAL ERROR: ");
        OutputDebugString(errMsg);
        OutputDebugString("\n");

        // Break the program, so we can check out what was going on.
        // HELLO! If you hit this breakpoint, then look at the debug
        // console, and go up the call stack to see what went wrong!
        DebugBreak();

        // You can try to keep running, if you want...
    }
    else {
        // Just dump a message box and terminate the app

        MessageBox(NULL, errMsg, "FATAL ERROR", MB_OK | MB_ICONERROR);
        ExitProcess(1);
    }
#else

    // Just dump it to printf and use exit.  On most OSs,
    // this is basically useless for debugging, so you'd
    // want to do better, especially under the debugger

    printf("FATAL ERROR: %s\n", errMsg);
    exit(1);

#endif
}

MYENGINE_NS_END
