//blockchain.h

typedef struct {	
	bool temp;
	char prevHash[65];
	char blockTitle[128];
	long long int blockNumber;
	time_t creationTime;
} Block;

typedef struct {
	bool temp;
	int transactionCount;
	char prevHash[65];
	char ownerKey[257];
	char license[129];
	time_t creationTime;
} Transaction;

Block recieveBlock(int);
void broadcastBlock(Block, string, vector<string>);
void saveBlockToFile(Block);
Block loadBlockFromFile(string);

Transaction recieveTransaction(int);
void broadcastTransaction(Transaction, string, vector<string>);
void saveTransactionToFile(Transaction);
Transaction loadTransactionFromFile(string);

int passiveTCP(const char*, int);
int connectTCP(const char*, const char*);

string sha256(string);
string sha256_file(string);

void reaper(int);

