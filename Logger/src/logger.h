/*
 * Log level configurator
 * Default is LOG_MAX_LEVEL_ERROR_WARNING_STATUS
 */ 

#include <errno.h>


#define LOG_MAX_LEVEL_ERROR 						          0
#define LOG_MAX_LEVEL_ERROR_WARNING_STATUS 			  1
#define LOG_MAX_LEVEL_ERROR_WARNING_STATUS_DEBUG 	2

#define LOG_LEVEL_ERROR 	  0
#define LOG_LEVEL_WARNING   1
#define LOG_LEVEL_STATUS 	  2
#define LOG_LEVEL_DEBUG 	  3
#define LOG_LEVEL_TELEMETRY	4
#define NUM_LOG_LEVELS		  5

/*
 * Prefixes for the different logging levels
 */
#define LOG_PREFIX_ERROR 		  "ERROR"
#define LOG_PREFIX_WARNING 	  "WARNING"
#define LOG_PREFIX_STATUS 	  "STATUS"
#define LOG_PREFIX_DEBUG 		  "DEBUG"
#define LOG_PREFIX_TELEMETRY  "TELEMETRY"

#if 0


static const char* LOG_LEVELS[NUM_LOG_LEVELS] = 
{ 
	LOG_PREFIX_ERROR,
    LOG_PREFIX_WARNING,
    LOG_PREFIX_STATUS,
    LOG_PREFIX_DEBUG
    LOG_PREFIX_TELEMETRY 
};


/*
 * Logger internal structure; move to class member elements.
 */
struct logger_t 
{
    int max_log_level;
    int use_stdout;
    FILE* out_file;
    void (*logger_func) (logger_t *context, const int level, const char*);
    logger_t()
    {
		max_log_level 	= LOG_MAX_LEVEL_ERROR;
		use_stdout		= 1;
		out_file		= NULL;
		logger_func		= NULL;
	}
};

/*
 * Logging methods by levels
 */
void log_error(logger_t *context, const char* format, ...);
void log_error(logger_t *context, int errnum, const char* format, ...);
void log_warning(logger_t *context, const char* format, ...);
void log_status(logger_t *context, const char* format, ...);
void log_debug(logger_t *context, const char* format, ...);

void logger_set_log_level(logger_t *context, const int level);

/*
 * Set target type
 * Default is syslog
 */
void logger_set_syslog(logger_t *context);
int logger_set_log_file(logger_t *context, const char* filename);
void logger_set_stdout(logger_t *context);

#else

class Logger // FileLogger SysLogger;
{
public:
    Logger(void);
    virtual ~Logger();
    void startLogger(void);
    void stopLogger(void);

    void log_error(const char* format, ...);
    void log_error(int32_t errnum, const char* format, ...);
    void log_warning(const char* format, ...);
    void log_status(const char* format, ...);
    void log_debug(const char* format, ...);

protected:
    FILE* out_file;
    bool bLogging;
    int32_t max_log_level;
    virtual void logger_func(const int32_t level, const char *makemessage);

  void log_generic(const int level, const char* format, va_list args);


private:
    static const char *LOG_LEVELS[NUM_LOG_LEVELS];

};

#endif