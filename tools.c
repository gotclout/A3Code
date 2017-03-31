#include "tools.h"

extern sms *smsp;

time_t now()
{
	return time(NULL);
}
long long time_ms() 
{
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long ms = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return ms;
}
char *parseTime(time_t time)
{
	char *tbuf=NULL;
	struct tm *tp= localtime(&time);
	struct tm t=*tp;	
	tbuf=(char*)malloc(30);
	sprintf(tbuf, "%d-%d-%d %d:%d:%d", t.tm_year+1900,t.tm_mon+1,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);
	return tbuf;
}
unsigned long long mstonano(long long ms) 
{
	unsigned long long ns=ms*1000000;
	return ns;
}
unsigned long long time_nano()
{
	return mstonano(time_ms());
}
int isInt(char *str) 
{
	char *pEnd;
	long int x = strtol(str, &pEnd, 10);
	if (pEnd == str) 
	{ 
		return 0;
	}
	return 1; 
}
int numlen(int n) 
{
	int cnt=0;
	while (n!=0) 
	{
		n/=10;
		cnt++;
	}
	return cnt;
}
int initSMS() 
{
	
	int smsid;
	key_t key = ftok("./Master.c", 60);
	//printf("Key=%d\n",key);
	int rqm = sizeof(sms);
	if ((smsid = shmget(key, rqm, IPC_CREAT | 0666)) == -1) 
	{
		perror("Error shmget()");
		exit(1);
	}
	if ((smsp = (sms*)shmat(smsid, NULL, 0)) == (void *)-1) 
	{
		perror("Error shmat");
		exit(1);
	}
	return smsid;
}

void add_sig_handler( void handle_signal()) 
{
	struct sigaction sigact;
	sigact.sa_handler = handle_signal;
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGSTOP, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);
	sigaction(SIGUSR1, &sigact, NULL);
	sigaction(SIGALRM, &sigact, NULL);
	sigaction(SIGABRT, &sigact, NULL);
}
char* tostring(int n, char* s, int rdx) 
{
	static char pattern[] ="0123456789abcdefghijklmnopqrstuvwxyz";
	char c,*p,*q;	
	unsigned int v;
	int i = 0, neg = 0;
	if (rdx == 10 && n < 0) 
	{
		n = -n;
		neg = 1;
	}
	v = n;
	do 
	{
		s[i++] = pattern[v%rdx];
		v /= rdx;
	}while (v);
	if (neg)
		s[i++] = '-';
	s[i] = '\0';
	for (p = s, q = p + (i-1); p < q; ++p, --q)
		c = *p, *p = *q, *q = c;
	return s;
}
void custom_delay(int ms)
{
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
}
UL rand_gen(UL start, UL end,unsigned seed)
{
	srand(seed);	     
       	double s = (double)rand();     
    	return (UL)((end-start +1)*s + start)%end;
}
UL sec2nano(int s) 
{
	UL nano = s * 1000000000;
	return nano;
}
float nano2sec(UL nano) 
{
	float s = (float)nano / 1000000000.0;
	return s;
}





