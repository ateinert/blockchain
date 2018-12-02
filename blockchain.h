//blockchain.h
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>

using namespace std;

class Block {
	public:
		Block();
		Block(int, string);
		bool validate();
		void setTempValue(bool);
		string getPrevHash();
		string getTitle();
		long long int getNumber();
		time_t getCreationTime();
		
	private:	
		bool temp;
		char prevHash[65];
		char blockTitle[128];
		long long int blockNumber;
		time_t creationTime;
};

class Transaction {
	public:
		Transaction();
		Transaction(int, string, string, string);
		bool validate();
		void setTempValue(bool);
		string getPrevHash();
		string getOwner();
		string getLicense();
		time_t getCreationTime();
	
	private:
		bool temp;
		int transactionCount;
		char prevHash[65];
		char ownerKey[257];
		char license[129];
		time_t creationTime;
};

void userInterface(ostream& os, int blockCount, int currentTransactionCount);
vector<Transaction> publishTransactions(string user_id, char* publisherID, char* gameID, int number, string socket, vector<string> hostList);

Block recieveBlock(int);
void broadcastBlock(Block, string, vector<string>);
void saveBlockToFile(Block);
Block loadBlockFromFile(string);

void loadBlockCount();
void updateBlockCount(int);
void loadTransactionCount();
void updateTransactionCount(int);
void loadWallet();
void updateWallet(vector<Transaction>);

Transaction recieveTransaction(int);
void broadcastTransaction(Transaction, string, vector<string>);
void saveTransactionToFile(Transaction);
Transaction loadTransactionFromFile(string);

void server();
void client(vector<string>, vector<Transaction>, string);

int passiveTCP(const char*, int);
int connectTCP(const char*, const char*);

string sha256(string);
string sha256_file(string);

void reaper(int);

