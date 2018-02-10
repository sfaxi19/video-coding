#ifndef GLOBAL
#define GLOBAL

#include <cstdio>
#include <cstring>
#include <chrono>
#include <map>

#define ANSI_FONT_BOLD     "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ERROR 0
#define WARN 1
#define INFO 2
#define TIMER 3
#define MAIN 4
static FILE *pLogFile = fopen("log.txt", "a+");

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG(msg, ...) { \
    if (pLogFile == nullptr) pLogFile = fopen("log.txt", "a+");\
    switch (msg) { \
        case ERROR: \
            printf(ANSI_COLOR_RED "ERROR:  \t"  "%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            fprintf(pLogFile, "ERROR:  \t%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            break;\
        case WARN:\
            printf(ANSI_COLOR_YELLOW "WARNING:\t"  "%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            fprintf(pLogFile, "WARNING:\t%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            break;\
        case INFO:\
            printf("INFO:   \t""%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            fprintf(pLogFile, "INFO:   \t""%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            break;\
        case TIMER:\
            printf(ANSI_COLOR_MAGENTA "TIMER:"  "  \t""%s\t%s[%d]:\t" , __TIME__, __FILENAME__, __LINE__);\
            fprintf(pLogFile, "TIMER:  \t""%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            break;\
        case MAIN:\
            printf(ANSI_COLOR_BLUE "MAIN:"  "   \t""%s\t%s[%d]:\t" , __TIME__, __FILENAME__, __LINE__);\
            fprintf(pLogFile, "MAIN:   \t""%s\t%s[%d]:\t", __TIME__, __FILENAME__, __LINE__);\
            break;\
        default:\
            printf("UNKNOWN::");\
    }\
    printf(__VA_ARGS__); \
    fprintf(pLogFile, __VA_ARGS__); \
    printf(ANSI_COLOR_RESET"\n"); \
    fprintf(pLogFile, "\n"); \
    if (msg == ERROR) { exit(1); } \
}

typedef std::map<uint32_t, std::chrono::time_point<std::chrono::steady_clock>> timers_map_t;
static timers_map_t timers_map;
static uint32_t timer_id = 0;

inline uint32_t timer_start() {
    LOG(TIMER, "TIMER:%d [start].", timer_id);
    timers_map.insert(timers_map_t::value_type(timer_id, std::chrono::steady_clock::now()));
    return timer_id++;
}

inline long timer_stop_us(uint32_t id) {
    auto end = std::chrono::steady_clock::now();
    auto timer = timers_map.find(id);
    auto ms = std::chrono::duration_cast<std::chrono::microseconds>(end - timer->second);
    timers_map.erase(timer);
    LOG(TIMER, "TIMER:%d [stop] Time=%ldus.", id, ms.count());
    return ms.count();
}

#endif //GLOBAL