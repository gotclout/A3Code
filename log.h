#include "tools.h"

enum LOG_TYPE {INFO,WARNING,ERROR};

typedef enum LOG_TYPE LOG_LEVEL;
typedef struct Logger
{
	time_t ts; //logging timestmap [optional]
	ULL nanots;// timestamp in nano seconds
	char *log_msg; // main log message
	LOG_LEVEL log_level; // type of log message
	struct Logger *next;
}logger;
logger *front,*rear;

char *build_name; // executable name contains bugs or exceptions
void buildLog();
char* logmsg(const char * format, ...);
logger *setLog(char *msg,LOG_LEVEL log_level);
int push_to_log(logger *log);
int save_log(char *file);
void reset_log();


