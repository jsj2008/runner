#include "crash_handler.h"

#ifdef __linux__

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static void sigsegv_handler(int sig)
{
   void *array[16] = { 0 };
   size_t size = backtrace(array, sizeof(array)/sizeof(array[0]));

   fprintf(stderr, "Error: signal %d:\n", sig);
   backtrace_symbols_fd(array, size, 2);
   exit(EXIT_FAILURE);
}

void crash_handler_init()
{
   signal(SIGSEGV, sigsegv_handler);
}

#elif WIN32

#include <windows.h>

void crash_handler_init()
{
   LoadLibraryW(L"libbacktrace-mingw.dll");
}

#else

#include <stdio.h>

void crash_handler_init()
{
   fprintf(stderr, "Unsupported platform. Unable to set crash handler.");
}

#endif
