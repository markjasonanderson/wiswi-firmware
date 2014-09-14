#ifndef __log_h__
#define __log_h__

#include "uart_if.h"

#define printf(M, ...) Report(M "\n\r", ##__VA_ARGS__)

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) Report("DEBUG %s:%d: " M "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)
#endif


#define log_err(M, ...) Report("[ERROR] (%s:%d) " M "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_warn(M, ...) Report("[WARN] (%s:%d) " M "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)

#define log_info(M, ...) Report("[INFO] (%s:%d) " M "\n\r", __FILE__, __LINE__, ##__VA_ARGS__)


#endif
