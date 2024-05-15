#include <syslog.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "logger.h"

/*
 * Program name variable is provided by the libc
 */
extern const char* __progname;

#define PROGRAM_NAME __progname

#if 0

#define LOG_LEVEL_ERROR 	0
#define LOG_LEVEL_WARNING 	1
#define LOG_LEVEL_STATUS 	2
#define LOG_LEVEL_DEBUG 	3
#define LOG_LEVEL_TELEMETRY	4
#define NUM_LOG_LEVELS		5

/*
 * Prefixes for the different logging levels
 */
#define LOG_PREFIX_ERROR 		"ERROR"
#define LOG_PREFIX_WARNING 		"WARNING"
#define LOG_PREFIX_STATUS 		"STATUS"
#define LOG_PREFIX_DEBUG 		"DEBUG"
#define LOG_PREFIX_TELEMETRY	"TELEMETRY"

static const char* LOG_LEVELS[NUM_LOG_LEVELS] = 
{ 
	LOG_PREFIX_ERROR,
    LOG_PREFIX_WARNING,
    LOG_PREFIX_STATUS,
    LOG_PREFIX_DEBUG,
    LOG_PREFIX_TELEMETRY 
};

static void print_to_syslog(logger_t *context, const int level, const char* message);

static void print_to_file(logger_t *context, const int level, const char* message);
										// Used for stdout too.
/*
 * Close remaining file descriptor and reset global params
 */
static void cleanup_internal(logger_t *context)
{
    if ( ( NULL!=context ) && ( NULL!=context->out_file ) ){
        if (!context->use_stdout) {
            (void)fclose(context->out_file);
        }

        context->use_stdout = 0;
        context->out_file 	= NULL;
    }
}

/*
 * Reset internal state and set syslog as default target
 */ 
void logger_set_syslog(logger_t *context)
{
    context->max_log_level = LOG_MAX_LEVEL_ERROR_WARNING_STATUS;
    cleanup_internal(context);
    context->logger_func = print_to_syslog;
}

/*
 * Print to syslog
 */
void print_to_syslog(logger_t *context, const int level, const char* message)
{
    (void)syslog(LOG_INFO, "[%s] %s\n", LOG_LEVELS[level], message);
}

/*
 * Print to file which can be a regular text file or STDOUT "file"
 */
static void print_to_file(logger_t *context, const int level, const char* message)
{
    struct tm* current_tm = NULL;
    time_t time_now;

    time(&time_now);
    current_tm = localtime(&time_now);

    int res = fprintf(context->out_file,
				"%s: %02i:%02i:%02i [%s] %s\n"
                , PROGRAM_NAME
                , current_tm->tm_hour
                , current_tm->tm_min
                , current_tm->tm_sec
                , LOG_LEVELS[level]
                , message );

    if (res == -1) {
        print_to_syslog(context, LOG_LEVEL_ERROR, "Unable to write to log file!");
        return;
    }

    (void)fflush(context->out_file);
}

/*
 */
void logger_set_log_level(logger_t *context, const int level)
{
    context->max_log_level = level;
}

/*
 */
int logger_set_log_file(logger_t *context, const char* filename)
{
    cleanup_internal(context);

    context->out_file = fopen(filename, "a");

    if (context->out_file == NULL) {		
        log_error(context, errno, "Failed to open file \"%s\"!", filename);
        return -1;
    }

    context->logger_func = print_to_file;

    return 0;
}

/*
 */
void logger_set_stdout(logger_t *context)
{
	if ( NULL==context )
	{
		return;
	}
	
    cleanup_internal(context);

    context->use_stdout = 1;
    context->logger_func= print_to_file;
    context->out_file 	= stdout;
}

/*
 * Logging functions
 */
void log_generic(logger_t *context, const int level, const char* format, va_list args)
{
    char buffer[256];
    (void)vsprintf(buffer, format, args);
    if ( NULL!=context->logger_func )
		context->logger_func(context, level, buffer);
}

void log_error(logger_t *context, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    log_generic(context, LOG_LEVEL_ERROR, format, args);
    va_end(args);
}

void log_error(logger_t *context, int errnum, const char *format, ...)
{	
	if ( NULL == context )
	{
		return;
	}
	
	if ( NULL != format )
	{
		va_list args;
			va_start(args, format);
			log_generic(context, LOG_LEVEL_ERROR, format, args);
		va_end(args);
	}

	// Log the system error:
	const char *fmt = "\r\t\"%s\"\r";
	char buf[256]; 
	size_t buflen = sizeof(buf);
	(void)strerror_r(errnum, buf, buflen);
    char buffer[256];
    (void)sprintf(buffer, fmt, buf);	
    if ( NULL!=context->logger_func )
		context->logger_func(context, LOG_LEVEL_ERROR, buffer);
	
}

void log_warning(logger_t *context, const char *format, ...)
{
    if (context->max_log_level < LOG_MAX_LEVEL_ERROR_WARNING_STATUS) {
        return;
    }

    va_list args;
    va_start(args, format);
    log_generic(context, LOG_LEVEL_WARNING, format, args);
    va_end(args);
}

void log_status(logger_t *context, const char *format, ...)
{
    if (context->max_log_level < LOG_MAX_LEVEL_ERROR_WARNING_STATUS) {
        return;
    }

    va_list args;
    va_start(args, format);
    log_generic(context, LOG_LEVEL_STATUS, format, args);
    va_end(args);
}

void log_debug(logger_t *context, const char *format, ...)
{
    if (context->max_log_level <  LOG_MAX_LEVEL_ERROR_WARNING_STATUS_DEBUG) {
        return;
    }

    va_list args;
    va_start(args, format);
    log_generic(context, LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

#else

const char *Logger::LOG_LEVELS[NUM_LOG_LEVELS] =
{
	LOG_PREFIX_ERROR,
    LOG_PREFIX_WARNING,
    LOG_PREFIX_STATUS,
    LOG_PREFIX_DEBUG,
    LOG_PREFIX_TELEMETRY 
};

Logger::Logger(void) :
    out_file(stdout), bLogging(false), max_log_level(LOG_LEVEL_ERROR)
{
    ;
}

Logger::~Logger()
{
    stopLogger();
}

void Logger::startLogger(void)
{
    bLogging = true;
}

void Logger::stopLogger(void)
{
    bLogging = false;
}

void Logger::logger_func(const int32_t level, const char *message)
{
    struct tm* current_tm = NULL;
    time_t time_now;

    time(&time_now);
    current_tm = localtime(&time_now);

    int32_t res = fprintf(out_file,
				"%s: %02i:%02i:%02i [%s] %s\n"
                , PROGRAM_NAME
                , current_tm->tm_hour
                , current_tm->tm_min
                , current_tm->tm_sec
                , LOG_LEVELS[level]
                , message );

    if ( 0 != res )
        perror("Unable to write to log file!");
    else    
        (void)fflush(out_file);
}

void Logger::log_generic(const int level, const char* format, va_list args)
{
    char buffer[256];
    (void)vsprintf(buffer, format, args);
    logger_func(level, buffer);
}


void Logger::log_debug(const char *format, ...)
{
    if (max_log_level <  LOG_LEVEL_DEBUG)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    log_generic(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

#endif