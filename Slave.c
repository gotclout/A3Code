#include "log.h"

#include "msgqlog.h"

void handle_signal(int signo);
void execute();
void packup();
int acquireLock (char *fileSpec);
void releaseLock (int lockFd);
char *outfile = "test.out",*buildname;
char *logfile;
int slvidx,done=0,qid;
int wcnt=DEFAULT_WRITES,smsid;
FILE *fp;
sms *smsp; 
int main(int argc, char **argv) 
{
	
	extern char* prog;
	int i=0;	
	signal(SIGINT, SIG_IGN);	
	
	add_sig_handler(*handle_signal);
	slvidx = atoi(argv[1]);	
	wcnt=rand_gen(10000,100000,getpid()); // slave run time
	logfile=argv[2];
	buildname=argv[3];
	build_name=argv[0];
	printf("slave=%d\tWc=%d\tlogfile=%s\tProg=%s\n",slvidx,wcnt,logfile,buildname);	
	smsid = initSMS();
	printf("share mem id=%d\nSlave Cnt=%d\n",smsid,smsp->slvcnt);
	smsp->slave_list[slvidx].pid = getpid();
	smsp->slave_list[slvidx].slvno = slvidx;
	smsp->slave_list[slvidx].ps = 1;
	smsp->slave_list[slvidx].wc = 0;
	if ((qid=initqueue(QKEY)) == -1) 
	{
		perror("Failed to initialize message queue");
		return 1;
	}
	setLog(logmsg("slave pid=%d\tSlave id=%d", getpid(),slvidx),INFO);
	execute();
	return 0;
}
void handle_signal(int signo)
{
	if (signo == SIGINT) 
	{
		setLog(logmsg("SIGINT @ slave id=%d", slvidx),INFO);
		done=1;
		packup();
		return;
	}
	else if (signo == SIGALRM) 
	{
		setLog(logmsg("SIGALRM @ slave id=%d", slvidx),INFO);
		packup();
	}
	else if (signo == SIGUSR1) 
	{
		setLog(logmsg("SIGUSR1 @ slave id=%d - Killed", slvidx),INFO);
		packup();
	}
}
void execute()
{
	printf("Execute @ Slave=%d\n",slvidx);
	custom_delay(time_ms()%2000);
	while(!done)
	{
		slave *slvp=&smsp->slave_list[slvidx];
		SysClk *clk=&smsp->clk;
		UL ns=sec2nano(clk->sec);
		ns=ns+clk->nano;
		if(slvp->wc>=wcnt)
		{
			slvp->ps=0;
			smsp->donecnt++;			
			done=1;
			char msg[1024];
			//sprintf(msg,"%d\t%u\t%lld",slvp->pid,clk->sec,clk->nano);
			sprintf(msg,"%d\t%u\t%llu\n",getpid(),clk->sec,clk->nano);
			printf("Slave=%d sending %s,Length=%d\n",getpid(),msg,strlen(msg));			
			if(msgwrite(msg, strlen(msg)) == -1)				
			{
				perror("Failed to write message");
				packup();
				//break;
			}	
			packup();
		}
		int fd;
		if((fd = acquireLock (outfile)) < 0) 
		{
		    //fprintf (stderr, "Cannot get lock file.\n");
		    //sleep(1);
		    custom_delay(time_ms()%2000);
		    continue;
		}
		setLog(logmsg("Slave id=%d Entered into CS @ %lld\n",slvidx,time_ms()),INFO);
		custom_delay(time_ms()%2000);
		smsp->shmnum++;
		fprintf(fp,"File Modified by Process Number %d at time %lld with shared number=%d\n",slvidx,time_ms(),smsp->shmnum);
		slvp->wc+=10000;
		custom_delay(time_ms()%2000);
		setLog(logmsg("Slave id=%d Exited from CS @ %lld\n",slvidx,time_ms()),INFO);
		releaseLock(fd);		
		custom_delay(time_ms()%2000);
	}
	
}

void packup() 
{	
	done=1;
	slave *slvp=&smsp->slave_list[slvidx];
	setLog(logmsg("Exiting Slave=%d\n",slvp->pid),INFO);
	slvp->ps=0;
	custom_delay(time_ms()%2000);
	save_log(logfile);
	exit(0);
}


int acquireLock (char *fileSpec) 
{
    int lockFd;
    fp=fopen(outfile,"a");	
    if (flock (fileno(fp), LOCK_EX | LOCK_NB) < 0) 
    {
        close (lockFd);
        return -1;
    }

    return lockFd;
}

void releaseLock (int lockFd) 
{
    flock (lockFd, LOCK_UN);
    fclose(fp);
    close (lockFd);    
}

