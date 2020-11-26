# Makefile, versao 1
# Sistemas Operativos, DEI/IST/ULisboa 2020-21

CC   = gcc
LD   = gcc
CFLAGS =-Wall -std=gnu99 -I../ -pthread -lpthread -ggdb
LDFLAGS=-lm

# A phony target is one that is not really the name of a file
# https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean run

all: tecnicofs

tecnicofs: fs/state.o fs/operations.o main.o fh/fileHandling.o thr/threads.o lst/list.o  er/error.o
	$(LD) $(CFLAGS) $(LDFLAGS) -o tecnicofs fs/state.o fs/operations.o fh/fileHandling.o thr/threads.o lst/list.o  er/error.o main.o

fs/state.o: fs/state.c fs/state.h er/error.h thr/threads.h lst/list.h tecnicofs-api-constants.h
	$(CC) $(CFLAGS) -o fs/state.o -c fs/state.c

fs/operations.o: fs/operations.c fs/operations.h fs/state.h er/error.h thr/threads.h lst/list.h tecnicofs-api-constants.h
	$(CC) $(CFLAGS) -o fs/operations.o -c fs/operations.c

fh/fileHandling.o: fh/fileHandling.h fh/fileHandling.c er/error.h
	$(CC) $(CFLAGS) -o fh/fileHandling.o -c fh/fileHandling.c

thr/threads.o: thr/threads.h thr/threads.c lst/list.h fs/state.h er/error.h
	$(CC) $(CFLAGS) -o thr/threads.o -c thr/threads.c

er/error.o: er/error.h er/error.c
	$(CC) $(CFLAGS) -o er/error.o -c er/error.c

lst/list.o: lst/list.h lst/list.c er/error.h
	$(CC) $(CFLAGS) -o lst/list.o -c lst/list.c

main.o: main.c fs/operations.h fs/state.h fh/fileHandling.h thr/threads.h lst/list.h er/error.h tecnicofs-api-constants.h 
	$(CC) $(CFLAGS) -o main.o -c main.c

clean:
	@echo Cleaning...
	rm -f fh/*.o thr/*.o er/*.o fs/*.o *.o tecnicofs

run: tecnicofs
	./tecnicofs
