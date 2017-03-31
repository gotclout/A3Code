#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PERM (S_IRUSR | S_IWUSR)
#define MAXSIZE 4096

typedef unsigned long long ULL;
typedef unsigned long UL;

typedef struct 
{
	long msgtype;
	char msg[MAXSIZE];
}msgq_data;
static int msgqid;
int initqueue(int key);
int msgwrite(void *buf, int len);
int msg_write(pid_t pid,unsigned int sec,ULL nano);




