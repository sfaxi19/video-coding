#ifndef GLOBAL
#define GLOBAL

#include <cstdio>
#include <cstring>
#include <chrono>
#include <map>

// Log colors:
#define ANSI_FONT_BOLD     "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
// Log levels:
#define ERROR 0
#define WARN  1
#define INFO  2
#define TIMER 3
#define MAIN  4

//extern FILE *pLogFile;

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

static FILE *pLogFile = nullptr;
#ifndef NDEBUG
#define LOG(msg, ...) { \
    if (pLogFile == nullptr) { \
        std::string filename = "log/"; \
        system(("mkdir -p " + filename).c_str()); \
        filename.append(__FILENAME__); \
        pLogFile = fopen(filename.substr(0, filename.size() - 4).append(".log").c_str(), "a+"); \
    } \
    std::chrono::system_clock::time_point PPPPP_TIMESTAMP = std::chrono::system_clock::now(); \
    std::time_t PPP_TIME = std::chrono::system_clock::to_time_t(PPPPP_TIMESTAMP); \
    std::string PPPPP_TIMETIME = std::ctime(&PPP_TIME); \
    PPPPP_TIMETIME = PPPPP_TIMETIME.substr(4, PPPPP_TIMETIME.length() - 10); \
    switch (msg) { \
        case ERROR:\
            printf(ANSI_COLOR_RED                   "ERROR:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "ERROR:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case WARN: \
            printf(ANSI_COLOR_YELLOW                "WARNING:\t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "WARNING:\t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case INFO: \
            printf(                                 "INFO:   \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "INFO:   \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case TIMER:\
            printf(ANSI_COLOR_MAGENTA               "TIMER:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "TIMER:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case MAIN: \
            printf(ANSI_COLOR_BLUE                  "MAIN:   \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "MAIN:   \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        default:   \
            printf("UNKNOWN::"); \
            break; \
    } \
    printf(__VA_ARGS__); \
    if (pLogFile) fprintf(pLogFile, __VA_ARGS__); \
    printf(ANSI_COLOR_RESET "\n"); \
    if (pLogFile) fprintf(pLogFile, "\n"); \
    if (msg == ERROR) { exit(1); } \
}

#else
#define LOG(msg, ...) { \
 if (msg != MAIN) { \
    if (pLogFile == nullptr) { \
        std::string filename = "log/"; \
        system(("mkdir -p " + filename).c_str()); \
        filename.append(__FILENAME__); \
        pLogFile = fopen(filename.substr(0, filename.size() - 4).append(".log").c_str(), "a+"); \
    } \
    std::chrono::system_clock::time_point PPPPP_TIMESTAMP = std::chrono::system_clock::now(); \
    std::time_t PPP_TIME = std::chrono::system_clock::to_time_t(PPPPP_TIMESTAMP); \
    std::string PPPPP_TIMETIME = std::ctime(&PPP_TIME); \
    PPPPP_TIMETIME = PPPPP_TIMETIME.substr(4, PPPPP_TIMETIME.length() - 10); \
    switch (msg) { \
        case ERROR:\
            printf(ANSI_COLOR_RED                   "ERROR:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "ERROR:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case WARN: \
            printf(ANSI_COLOR_YELLOW                "WARNING:\t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "WARNING:\t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case INFO: \
            printf(                                 "INFO:   \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "INFO:   \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        case TIMER:\
            printf(ANSI_COLOR_MAGENTA               "TIMER:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            if (pLogFile) fprintf(pLogFile,         "TIMER:  \t%s\t%20s[%4d]:\t\t", PPPPP_TIMETIME.c_str(), __FILENAME__, __LINE__);\
            break; \
        } \
        printf(__VA_ARGS__); \
        if (pLogFile) fprintf(pLogFile, __VA_ARGS__); \
        printf(ANSI_COLOR_RESET "\n"); \
        if (pLogFile) fprintf(pLogFile, "\n"); \
        if (msg == ERROR) { exit(1); } \
    } \
 }
#endif

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