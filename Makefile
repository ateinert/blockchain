CXX=g++
#INCS= -I. -I/usr/include # to use HPUX with gcc
OBJECTS= block.o transaction.o sha256.o connectTCP.o connectsock.o passivesock.o passiveTCP.o

CFLAGS = -g -Wall -std=c++11

.SUFFIXES: .cpp

all: blockchain

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

blockchain: main.o $(OBJECTS)
	$(CXX) $(CFLAGS) main.o $(OBJECTS) \
	-o main

clean:
	rm -f *.o core *~ blockchain
