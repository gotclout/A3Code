CC = gcc
CFLAGS = -g -lm -pthread -lgmp -lreadline
.SUFFIXES: .c .o
all: msgqutil tools logger oss slave

msgqutil:
	$(CC) -c -o msgqutil.o msgqutil.c
tools:
	$(CC) -c -o tools.o tools.c
logger: tools.o
	$(CC) -c -o logger.o logger.c
oss: msgqutil.o logger.o tools.o
	$(CC) $(CFLAGS) -o oss msgqutil.o logger.o tools.o Master.c
slave: 	msgqutil.o logger.o tools.o
	$(CC) $(CFLAGS) -o Slave msgqutil.o logger.o tools.o Slave.c
clean:
	/bin/rm -rf *.o logger oss Slave *.dSYM *.log
