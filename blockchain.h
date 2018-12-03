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
	char transactionTitle[256];
	char prevHash[65];
	char ownerKey[257];
	char license[129];
	time_t creationTime;
} Transaction;

void server(char*);

int blockValidate(Block);
int transactionValidate(Transaction);

Block createBlock(int, char[65]);
Block createTransaction(int, int, char[65], char[257], char[129);

Block recieveBlock(int);
void broadcastBlock(Block, char**, char*, int);
void saveBlockToFile(Block);
Block loadBlockFromFile(char*);

Transaction recieveTransaction(int);
void broadcastTransaction(Transaction, char**, char*, int);
void saveTransactionToFile(Transaction);
Transaction loadTransactionFromFile(char*);

int passiveTCP(const char*, int);
int connectTCP(const char*, const char*);

void sha256(char *string, char outputBuffer[65]);
int sha256_file(char *path, char outputBuffer[65]);

void reaper(int);

