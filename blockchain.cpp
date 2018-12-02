//blockchain.cpp
#include <vector>
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

void loadWallet()
{
	FILE *file;
	file = fopen("chain/wallet", "r");
	wallet.clear();
	Transaction temp;
	while (fread(&temp, sizeof(temp), 1, file))
	{
		wallet.push_back(temp);
	}
	fclose(file);
}

void updateWallet(vector<Transaction> list)
{
	FILE *file;
	file = fopen("chain/wallet", "w");
	for (int i = 0; i < list.size(); i++)
	{
		fwrite (&list[i], sizeof(Transaction), 1, file); 
	}
	fclose(file);
	wallet = list;
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
		exit(1);
	if (block.validate())
	{
		saveBlockToFile(block);
		if (write(fd, validMssg, strlen(validMssg)) < 0)
			exit(1);
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
			exit(1);
		
		while (cc = read(s, buf, sizeof buf)) 
		{
			if (cc < 0)
				exit(1);
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
		exit (1); 
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
		exit(1);
	if (trans.validate())
	{
		saveTransactionToFile(trans);
		if (write(fd, validMssg, strlen(validMssg)) < 0)
			exit(1);
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
			exit(1);
		
		while (cc = read(s, buf, sizeof buf)) 
		{
			if (cc < 0)
				exit(1);
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
		exit (1); 
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

void userInterface(ostream& os, int blockCount, int currentTransactionCount)
{
	os << "\033[1J";
	os << "\033[1;1H";
	os << "Current Block Count: " << setw(7) << " " << blockCount << endl;
	os << "Current Transaction Count: " << currentTransactionCount << endl;
	//os << "Most Recent Event: " << recentEvent << endl;
}

vector<Transaction> publishTransactions(string id, char *publisherID, char *gameID, int number, string socket, vector<string> hostList)
{
	char license[129];
	char key[65];
	int len = 64;
	static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
	vector<Transaction> t;
	loadTransactionCount;
	for (int i = 0; i < number; i++)
	{
		for (int j = 0; j < len; ++j) {
			key[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
		}
		key[64] = '\0';
		strcat(license, publisherID);
		strcat(license, gameID);
		strcat(license, key);
		
		license[128] = '\0';
		Transaction temp;
		//publish license using transaction
		broadcastTransaction(temp, socket, hostList);
		t.push_back(temp);
		updateTransactionCount(transactionCount+1);
	}
	return t;
}

void client(vector<string> hosts, vector<Transaction> wallet, string service)
{
	// this will only send and create new transactions automatically no user input;
	// occurs every 3 seconds
	loadTransactionCount();
	updateTransactionCount(transactionCount+1);
	Transaction temp;
	// then point it to the previous transactions
	broadcastTransaction(temp, service, hosts);
}

void server()
{
	// child server	
	// create different ports for recieving transactions, blocks and the chain settings requests
	int msock1, msock2;
	int ssock;
	int alen;
	sockaddr_in fsin;
	
	// listen for transaction
	msock1 = passiveTCP(TRANSACTION_PORT, QLEN);
	// listen for block
	msock2 = passiveTCP(BLOCK_PORT, QLEN);
	// listen for chain
	
	while (1)
	{
		alen = sizeof(fsin);
		// fork for each one
		if (fork() == 0)
		{
			//child 1
			ssock = accept(msock1, (sockaddr *)&fsin, (socklen_t *)&alen);
			if (ssock < 0) 
			{
				if (errno == EINTR)   //system call was interrupted permaturely with a signal before it was able to complete
					continue;
				cerr << "socket failure" << endl;
				exit(EXIT_FAILURE);
			}
			switch (fork()) 
			{
				case 0:		/* child */
					(void) close(msock1);
					recieveTransaction(ssock);
					exit(0);
				default:	/* parent */
					(void) close(ssock);
					break;
				case -1:
					exit(1);
			}
		}
		else
		{
			if (fork() == 0)
			{
				//child 2
				ssock = accept(msock2, (sockaddr *)&fsin, (socklen_t *)&alen);
				if (ssock < 0) 
				{
					if (errno == EINTR)   //system call was interrupted permaturely with a signal before it was able to complete
						continue;
					cerr << "socket failure" << endl;
					exit(EXIT_FAILURE);
				}
				switch (fork()) 
				{
					case 0:		/* child */
						(void) close(msock2);
						recieveBlock(ssock);
						exit(0);
					default:	/* parent */
						(void) close(ssock);
						break;
					case -1:
						exit(1);
				}
			}
			else
			{
				break;
			}
			break;
		}
	}
}

void reaper(int sig)
{
	int	status;

	while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0)
		/* empty */;
}
