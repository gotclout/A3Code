#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <locale.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/file.h>

#define   LOCK_SH   1    /* shared lock */
#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */
#define   LOCK_UN   8    

#define DEFAULT_WRITES 3
#define DEFAULT_SLAVES 5
#define DEFAULT_EXE_TIME 60
#define QKEY 1234

typedef unsigned long long ULL;
typedef unsigned long UL;

typedef struct
{
	pid_t pid;
	int slvno;
	int wc;// write count so far
	int ps; //process status
}slave;

typedef struct
{
	unsigned int sec;
	ULL nano;
}SysClk;

typedef struct shared_mem_struct
{
	slave slave_list[10];
	int shmnum;
	int donecnt;
	int slvcnt;// slave count
	int lockstat;
	SysClk clk;
}sms; //shared memory structure

time_t now();// current system time in time_t structure format
long long time_ms(); // current time in milliseconds
ULL mstonano(long long ms);//convert millis to nanos
ULL time_nano(); // current time in nano seconds
char *parseTime(time_t time); //converts time format to string format
int isInt(char *str); // whether given input is interger or not
int numlen(int n); // find number of digits in given number
int initSMS();
void add_sig_handler( void handle_signal());
char* tostring(int num, char* str, int radix);
UL rand_gen(UL start, UL end,unsigned seed);
void custom_delay(int ms);
UL sec2nano(int s);
float nano2sec(UL nano);




 
