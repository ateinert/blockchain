//blockchain.cpp
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <netinet/in.h>
#include "blockchain.h"

#define LINELEN	128
#define BLOCK_PORT "2000"
#define TRANSACTION_PORT "2001"
#define QLEN 5

using namespace std;

extern int blockCount, transactionCount;
extern vector<Transaction> wallet;

void loadBlockCount()
{
	//load it into memory
	ifstream file;
	file.open("chain/blockCount");
	file >> blockCount;
}

void updateBlockCount(int count)
{
	// open the file and set it equal to count
	ofstream file;
	file.open("chain/blockCount");
	file << count;
	//then load it into memory
	blockCount = count;
}

void loadTransactionCount()
{
	ifstream file;
	file.open("chain/transactionCount");
	file >> transactionCount;
}

void updateTransactionCount(int count)
{
	ofstream file;
	file.open("chain/transactionCount");
	file << count;
	transactionCount = count;
}

Block recieveBlock(int fd)
{
	loadBlockCount();
	updateBlockCount(blockCount + 1);
	updateTransactionCount(0);
	Block block;
	char buf[BUFSIZ];
	char endOfFile_Indicator[]="End of file\n";
	char validMssg[]="Block Valid\n";
	int	cc;
	
	cc = read(fd, &block, sizeof(Block));
	//printf("recieved: %s, %d\n", buf,cc);
	if (cc < 0)
	{
		cerr << "ERROR 1\n";
		exit(1);
	}
	if (block.validate())
	{
		saveBlockToFile(block);
		if (write(fd, validMssg, strlen(validMssg)) < 0)
		{
			cerr << "ERROR 2\n";
			exit(1);
		}
		return block;
	}
}

void broadcastBlock(Block block, string sock, vector<string> hosts)
{
	
	char buf[LINELEN+1];		/* buffer for one line of text	*/
   	char endoffile[]="End of file\n";
	int	s, i, n, cc;			/* socket descriptor, read count*/
	int	outchars, inchars;	/* characters sent and received	*/
	char validMssg[]="Block Valid\n";
	for (i = 0; i < hosts.size(); i++)
	{
		char *host = (char*)hosts[i].c_str();
		s = connectTCP(host, sock.c_str());
		if (write(s, &block, sizeof(Block)) < 0)
		{
			cerr << "ERROR 3\n";
			exit(1);
		}
		while (cc = read(s, buf, sizeof buf)) 
		{
			if (cc < 0)
			{
				cerr << "ERROR 4\n";
				exit(1);
			}
			if(strncmp(buf,validMssg,strlen(validMssg))==0) 
			{
				//printf("File Recieved and Verified\n");
				saveBlockToFile(block);
				break;
			}
		}
	}
}

void saveBlockToFile(Block block)
{
	FILE *file;
	string s = "chain/blocks/block" + block.getTitle();
	mkdir(s.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	s = s + "/" + block.getTitle();
	file = fopen(s.c_str(), "w");
	//printf("Attempting to save block: %s\n", block.blockTitle);
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		{
			cerr << "ERROR 5\n";
			exit(1);
		} 
	}
	fwrite (&block, sizeof(Block), 1, file); 
	if(fwrite == 0)  
	{
		printf("Error writing file\n"); 
		exit (1);
	}
	fclose(file);
}

Block loadBlockFromFile(string str)
{
	Block block;
	FILE *file;
	file = fopen(str.c_str(), "r");
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		exit (1); 
	}
	while(fread(&block, sizeof(Block), 1, file));
	// close file 
	fclose(file); 
	return block;
}

Transaction recieveTransaction(int fd)
{
	loadTransactionCount();
	updateTransactionCount(transactionCount + 1);
	Transaction trans;
	char buf[BUFSIZ];
	char endOfFile_Indicator[]="End of file\n";
	char validMssg[]="Transaction Valid\n";
	int	cc;
	
	cc = read(fd, &trans, sizeof(Transaction));
	//printf("recieved: %s, %d\n", buf,cc);
	if (cc < 0)
	{
		cerr << "ERROR 6\n";
		exit(1);
	} 
	if (trans.validate())
	{
		saveTransactionToFile(trans);
		if (write(fd, validMssg, strlen(validMssg)) < 0)
		{
			cerr << "ERROR 7\n";
			exit(1);
		} 
		return trans;
	}
}

void broadcastTransaction(Transaction trans, string sock, vector<string> hosts)
{
	char buf[LINELEN+1];		/* buffer for one line of text	*/
   	char endoffile[]="End of file\n";
	int	s, i, n, cc;			/* socket descriptor, read count*/
	int	outchars, inchars;	/* characters sent and received	*/
	char validMssg[]="Transaction Valid\n";
	for (i = 0; i < hosts.size(); i++)
	{
		char *host = (char*)hosts[i].c_str();
		s = connectTCP(host, sock.c_str());
		if (write(s, &trans, sizeof(Transaction)) < 0)
		{
			cerr << "ERROR 8\n";
			exit(1);
		} 
		
		while (cc = read(s, buf, sizeof buf)) 
		{
			if (cc < 0)
			{
				cerr << "ERROR 9\n";
				exit(1);
			} 
			if(strncmp(buf,validMssg,strlen(validMssg))==0) 
			{
				saveTransactionToFile(trans);
				//printf("File Recieved and Verified\n");
				break;
			}
		}
	}
}

void saveTransactionToFile(Transaction trans)
{
	FILE *file;
	loadBlockCount();
	string s = "chain/blocks/block" + to_string(blockCount) + "/" + to_string(transactionCount);
	file = fopen(s.c_str(), "w");
	//printf("Attempting to save block: %s\n", block.blockTitle);
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		{
			cerr << "ERROR 10\n";
			exit(1);
		} 
	}
	fwrite (&trans, sizeof(Transaction), 1, file); 
	if(fwrite == 0)  
	{
		printf("Error writing file\n"); 
		exit (1);
	}
	fclose(file);
}

Transaction loadTransactionFromFile(string str)
{
	Transaction trans;
	FILE *file;
	file = fopen(str.c_str(), "r");
	if (file == NULL) 
	{ 
		fprintf(stderr, "\nError opening file\n"); 
		exit (1); 
	}
	while(fread(&trans, sizeof(Transaction), 1, file));
	// close file 
	fclose(file); 
	return trans;
}

void reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
