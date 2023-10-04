#include <stdio.h>

#if defined(DEBUG)
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

#if defined(DEBUG)
    #define WARNING_PRINT(fmt, ...) fprintf(stderr, "[WARNING] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define WARNING_PRINT(fmt, ...)
#endif

#if defined(DEBUG)
    #define ERROR_PRINT(fmt, ...) fprintf(stderr, "[ERROR] %s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define ERROR_PRINT(fmt, ...)
#endif