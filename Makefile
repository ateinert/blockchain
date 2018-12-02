#
CXX=g++
RM= \rm -rf
GZIP=gzip
TAR=tar cvf

INCS= -I. 
#INCS= -I. -I/usr/include # to use HPUX with gcc
OPTFLAGS=  -O4 -lssl -lcrypto -std=c++11
OBJECTS= block.o transaction.o sha256.o
OBJECTS1= connectTCP.o connectsock.o 
OBJECTS2= passivesock.o passiveTCP.o
OBJECTS3= main.o

DISTRIB = main.cpp blockchain.cpp connectTCP.cpp connectsock.cpp sha256.cpp block.cpp transaction.cpp

CFLAGS = $(INCS) $(OPTFLAGS)

.SUFFIXES: .cpp

all: blockchain

.c.o:
	$(CXX) $(CFLAGS) -c $<

blockchain: main.o $(OBJECTS2) $(OBJECTS1) $(OBJECTS) $(OBJECTS3)
	$(CXX) $(CFLAGS) main.o $(OBJECTS2) $(OBJECTS1) $(OBJECTS) $(OBJECTS3) \
	-o main

clean:
	rm -f *.o core *~ blockchain
