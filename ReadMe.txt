Executable : oss
----------
Usage
-----
	oss [-s slave-count] [-h] [-l <logfile>][-t execution-time(in sec)]
	

Files Description
-----------------	

log.h : Logging utility functions header file

functions:
	buildLog()	: allocate memory to new log entry
	logmsg()	: take variable number of args like printf() to build log message buffer
	setLog()	: build entry with given log message and log level
	push_to_log()	: add created entry to existing log collection	
	save_log()	: export in memory log to a file

tools.h : useful methods collection

Master.c : This is the main program to execute other build with buggy code.

functions:

	void usage() : display help about options
	int process_options(int argc,char *argv[]) : interpret input options
	void handle_signal(int signo) : handling ^C,ALRAM like interrupts
	void packup() : deallocate all resources and exit
	int freeShm(int shmid, void *shmaddr) : detach shared memory
	void invoke_slaves() : initialize slave processes sequentially with small time gap
	void invoke_slave(int slvidx) : invoke particular slave process
	void init_master() : to allocate shared memory,start slave processes and initialize logs and other data structures if any
	void throw_signal(int signo) : to forward or send signal to slave processes 
	pid_t rpc_wait(int *stat) : wait for signal handling
	void watch_dog() : monitor method for slaves processes and quit if they are completed.

Slave.c	: 
---------
	void handle_signal(int signo): handling ^C,ALRAM like interrupts from Master Process
	void execute()	: Action part of this process. Critical Section logic is placed here.Internally it hook the lock and do some process and release that lock 
	void packup()	: deallocate all resources and exit
	int acquireLock (char *fileSpec) : To gain control on the file to be modified
	void releaseLock (int lockFd) : To release control on the file after modification.

Note: We used messge queue functions from robin&robin text book(msgqlog.h,msgqutil.c) with little modification	

Before Run
-----------
$make clean
$make
Sample Run
-----------
	$ oss
Output:

	Log file=logfile.txt
	Slave cnt=5
	Exe Time=60
	Wcnt=3
	In init_master()
	Shared memid=2162691
	slave=1	Wc=80214	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=1
	Execute @ Slave=1
	slave=2	Wc=76756	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=2
	Execute @ Slave=2
	slave=3	Wc=5696	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=3
	Execute @ Slave=3
	slave=4	Wc=22514	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=4
	Execute @ Slave=4
	slave=5	Wc=78190	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=5
	Execute @ Slave=5
	invoke_slaves() with slvcnt=5
	watch_dog()
	Slave=8985 sending 8985	0	850217398
	,Length=17
	OSS Received=8985	0	850217398
	,Length=17
	slave=1	Wc=4889	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=6
	Execute @ Slave=1
	Slave=8989 sending 8989	1	768452458
	,Length=17
	OSS Received=8989	1	768452458
	,Length=17
	slave=2	Wc=85882	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=7
	Execute @ Slave=2
	Slave=8986 sending 8986	2	609799201
	,Length=17
	OSS Received=8986	2	609799201
	,Length=17
	slave=4	Wc=36566	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=8
	Execute @ Slave=4
	Slave=8983 sending 8983	3	3149876
	,Length=15
	OSS Received=8983	3	3149876
	1
	,Length=17
	slave=3	Wc=92016	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=9
	Execute @ Slave=3
	Slave=8982 sending 8982	3	814584190
	,Length=17
	OSS Received=8982	3	814584190
	,Length=17
	slave=2	Wc=82282	logfile=logfile.txt	Prog=oss
	share mem id=2162691
	Slave Cnt=10
	Execute @ Slave=2


