#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

#include "os_core.h"

// System calls act as the bridge between the process and the OS resources
void perform_system_call(const char* type, const char* arg1, const char* arg2);

#endif