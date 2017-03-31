#include "log.h"

#include "msgqlog.h"


char *logfile = "logfile.txt";
char *buildname;
sms *smsp; 
void watch_dog();
void usage();
int process_options(int argc,char *argv[]);
void handle_signal(int signo);
void packup();
int freeShm(int shmid, void *shmaddr);
void invoke_slaves();
void invoke_slave(int slvidx);
void init_master();
void throw_signal(int signo);
pid_t rpc_wait(int *stat);
void fwd_clock(unsigned int ns);
int freeSlot();

int exetime=DEFAULT_EXE_TIME,wcnt=DEFAULT_WRITES,smsid,slvcnt=DEFAULT_SLAVES;
int qid;
void main(int argc,char *argv[])
{
	system("clear");
	buildname=argv[0];
	build_name=argv[0];
	process_options(argc,argv);
	printf("Log file=%s\nSlave cnt=%d\nExe Time=%d\nWcnt=%d\n",logfile,slvcnt,exetime,wcnt);
	add_sig_handler(*handle_signal);
	alarm(exetime);
	init_master();
	watch_dog();
	
}
void usage()
{
	if(buildname!=NULL)
		printf("Usage: %s [-h] [-l <log-file-name>][-s <slave-count>][-t <execution-time(in sec)>]\n",buildname);
}
int process_options(int argc,char *argv[])
{
	int ch,i;
	char msg[100];
	int index,status=0;
	opterr = 0;	
	while ((ch = getopt (argc, argv, "hs:l:i:t:")) != -1)
	{		
		switch (ch)
           	{           		
           		case 'l':
           			logfile=optarg;
           		break;
           		case 'h':           			
	           		usage();
	           		exit(0);
           		break;
			case 's':           			
           			if(!isInt(optarg))
	           			fprintf(stderr,"Wrong Input for switch s. Slave count must be Positive Integer");
				else
				{	           			
           				slvcnt=atoi(optarg);
           				if(slvcnt<=0)
           				{
           					fprintf(stderr,"Wrong Input for switch s. Slave count must be Positive Integer");
           					slvcnt=DEFAULT_SLAVES;
           				}
					else           
					{
						slvcnt=(slvcnt==0)?DEFAULT_SLAVES:slvcnt;
					}					
           			}
           		break;
           		case 't':
           			if(!isInt(optarg))
	           			fprintf(stderr,"Wrong Input for switch t. Execution Time must be Positive Integer");
				else
				{	           			
           				exetime=atoi(optarg);
           				if(exetime<=0)
           				{
           					fprintf(stderr,"Invalid Input for Option -t. Input (termination time) must be positive Integer");
           					exetime=DEFAULT_EXE_TIME;
           				}
					else           
					{
						exetime=(exetime==0)?DEFAULT_EXE_TIME:exetime;
					}					
           			}
           		break;           		
           		case '?':
           			if (optopt == 'l')
				{
					fprintf(stderr,"Invalid Usage:");
					fprintf(stderr,"Option -%c requires an argument (log filename).\n", optopt);
					usage();					
				}
				else if (optopt == 's')
				{					
					fprintf(stderr,"Invalid Usage:");
					fprintf(stderr,"Option -%c requires an argument (slave count).\n", optopt);
					usage();					
				}				
				else if (optopt == 't')
				{					
					fprintf(stderr,"Invalid Usage:");
					fprintf(stderr,"Option -%c requires an argument (execution time).\n", optopt);
					usage();					
				}

				else if (isprint (optopt))
			       		fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			       	else
			       		fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
				return -1;				
			default:
				usage();

           	}
        }       
	return 0;

}
pid_t rpc_wait(int *stat) 
{
	int rval;
	while (((rval = wait(stat)) == -1) && (errno == EINTR));
	return rval;
}

void throw_signal(int signo) 
{
	int i;
	for (i=0; i<slvcnt; i++) 
	{
		if (smsp->slave_list[i].ps == 1) 
		{
			kill(smsp->slave_list[i].pid, signo);
			rpc_wait(NULL);
		}
	}		
}

void handle_signal(int signo) 
{
	if (signo == SIGINT) 
	{
		setLog(logmsg("SIGINT @ Master - Exiting.\n"),INFO);
		throw_signal(SIGUSR1);
		packup();		
	}
	else if (signo == SIGALRM) 
	{
		throw_signal(signo);
		setLog(logmsg("Timeout @ Master.\n"),INFO);
	}
	else if (signo == SIGUSR1)
		return;
	packup();
}
void packup()
{
	setLog(logmsg("Total Slaves Created=%d\nTotal Done=%d\nInterrupted=%d\n",smsp->slvcnt,smsp->donecnt,(smsp->slvcnt-smsp->donecnt)),INFO);
	setLog(logmsg("Packup @ Master.Exiting..."),INFO);
	freeShm(smsid,smsp);
	save_log(logfile);
	exit(0);	
}

int freeShm(int shmid, void *shmaddr) 
{
	int error = 0;
	if (shmdt(shmaddr) == -1)
	{
		error = errno;
		perror("Error in shmdt()");
	}
	if ((shmctl(shmid, IPC_RMID, NULL) == -1) && !error)
	{
		error = errno;
		perror("Error in shmctl()");
	}
	if (!error)
		return 0;
	errno = error;
		return -1;
}
void init_master()
{
	char *outfile = "test.out";	
	printf("In init_master()\n");
	smsid = initSMS();
	printf("Shared memid=%d\n",smsid);
	SysClk *clk=&smsp->clk;
	clk->sec=0;
	clk->nano=0;
	if ((qid = initqueue(QKEY)) == -1) 
	{
		perror("Failed to initialize message queue");
		return;
	}
	setLog(logmsg("Shared Memory Id=%d\nMsgQ Id=%d",smsid,qid),INFO);
	FILE *fp = fopen(outfile, "w");
	fclose(fp);
	fp = fopen(logfile, "w");
	fclose(fp);		
	smsp->shmnum=0;	
	invoke_slaves();
}
void invoke_slave(int slvidx) 
{
	int pid;	
	pid = fork();
	if ( pid == 0 ) 
	{
		char *slvno = malloc(numlen(slvidx+1));
		char *wc=malloc(numlen(wcnt));		
		sprintf(slvno, "%d", slvidx+1);
		sprintf(wc, "%d", wcnt);
		execl("./Slave", "Slave", slvno,logfile,buildname,NULL); 
		perror("Error Slave");
	}
	else if (pid == -1 ) 
	{
		perror("Failed to fork.");
		sleep(1);
	}
}


void invoke_slaves()
{
	int sidx;
	for(sidx=0;sidx<slvcnt;sidx++)
	{			
		smsp->slvcnt++;
		invoke_slave(sidx);
		sleep(1);					
	}	
	printf("invoke_slaves() with slvcnt=%d\n",smsp->slvcnt);
}
int freeSlot()
{
	int sidx,slot=-1;
	for(sidx=0;sidx<slvcnt;sidx++)
	{
		if(smsp->slave_list[sidx].ps==0)
		{
			slot=sidx;
			smsp->slave_list[sidx].ps=1;
			break;
		}
	}	
	return slot;
}
void fwd_clock(unsigned int ns) 
{
	smsp->clk.nano+=ns;
	if (smsp->clk.nano>= 1000000000) 
	{ 
		smsp->clk.nano -= 1000000000; 
		smsp->clk.sec++;
	}	
}
void watch_dog()
{
	int done=0,it=1000,size;
	msgq_data qdata;
	char *p;
	printf("watch_dog()\n");	
	while(!done)
	{
		int alldone=1;
		int idx;
		int slot=-1,curpid=0;		
		unsigned int slvsec=0;
		ULL slvns=0;
		char ch1,ch2;
		unsigned int ns = rand_gen(100000000,1000000000,it);					
		fwd_clock(ns); 		
		if ((size = msgrcv(qid, &qdata, MAXSIZE, 0, 0)) == -1) 
		{
			perror("Failed to read message queue");			
			break;
		}
		printf("OSS Received=%s,Length=%d\n",qdata.msg,strlen(qdata.msg));
		fflush(stdin);
		sscanf(qdata.msg,"%d%u%llu",&curpid,&slvsec,&slvns);		
		//sscanf(qdata.msg,"%d%u\t\t%u%llu",&curpid,&slvsec,&slvsec,&slvns);
		setLog(logmsg("OSS:Child %d is terminating at my time %u.%llu because it reached %u.%llu in slave",curpid,smsp->clk.sec,smsp->clk.nano,slvsec,slvns),INFO);
		slot=freeSlot();						
		if(slot!=-1)
		{
			smsp->slvcnt++;
			invoke_slave(slot);		
		}		
		it+=100;
		sleep(1);			
	}	
}

