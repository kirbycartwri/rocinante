#include <syslog.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"

/*
 * 
 *  Compilation: g++ -Wall logger_example.cpp ../src/logger.cpp -I ../src -I ./ -o logger_example -g
 */

int main( void )
{
	logger_t my_log;
	
	logger_set_syslog(&my_log);

	log_warning(&my_log, "This message goes to syslog");

	logger_set_stdout(&my_log);

	log_status(&my_log, "Hello!");

	logger_set_log_file(&my_log, "log.txt");

	log_error(&my_log, "Logger in a file mode!");

	return 0;
}

