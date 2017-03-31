#include "msgqlog.h"
int initqueue(int key) 
{ 
	msgqid = msgget(key, PERM | IPC_CREAT);
	if (msgqid == -1)
		return -1;
	return 0;
}
int msgwrite(void *buf, int len) 
{ 	
	msgq_data *datap;
	int error = 0;
	if ((datap = (msgq_data *)malloc(sizeof(msgq_data) + len - 1)) == NULL)
		return -1;
	//memcpy(datap->msg, buf, len);
	strcpy(datap->msg,buf);
	datap->msgtype = 1; 
	if (msgsnd(msgqid, datap, len, 0) == -1)
		error = errno;
	free(datap);
	if (error) 
	{
		errno = error;
		return -1;
	}
	return 0;
}







