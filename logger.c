#include "log.h"
logger *entry=NULL;
int entcnt=0;// Total Log Entries

void buildLog()
{	
	entry=(logger*)malloc(sizeof(logger));	
	entry->next=NULL;
	entry->ts=now();
	//entry->nanots=time_nano();
	entry->nanots=time_ms();
	entcnt++;
}
char* logmsg(const char * format, ...) 
{			
		char *buffer=malloc(256);
		va_list args;
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);	
		//printf("Buf=%s",buffer);
		return buffer;		
}
logger *setLog(char *msg,LOG_LEVEL level)
{
	//printf("len of msg=%d\nmsg=%s\n",strlen(msg),msg);
	buildLog();
	entry->log_msg=malloc(strlen(msg)+5);
	strcpy(entry->log_msg,msg);
	entry->log_level=level;
	push_to_log(entry);
	return entry;
}
int push_to_log(logger *log) 
{	
	if(front==NULL)
	{
		front=rear=entry;
		return 0;
	}
	rear->next=entry;
	rear=entry;	
	return 0;
}

int save_log(char *logf) 
{
	char *levels[]={"INFO","WARNING","ERROR"};
	FILE *fileptr=fopen(logf,"a");
	logger *logptr=NULL;	
	char *timestr=NULL;
	for(logptr=front;logptr;logptr=logptr->next)
	{
		//timestr=parseTime(logptr->ts);		
		fprintf(fileptr,"%s:%lld->%s - %s\n",build_name,logptr->nanots,levels[logptr->log_level],logptr->log_msg);
	}
	fclose(fileptr);
	return 0;
}
void reset_log() 
{
	entcnt=0;
	free(front);
	front=NULL;
	free(rear);
	rear=NULL;
}








