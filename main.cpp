//main.cpp

//sudo apt install libssl-dev

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <signal.h>
#include <stdlib.h>
#include "blockchain.h"

#define BLOCK_PORT "2000"
#define TRANSACTION_PORT "2001"

char PUBLISHER_ID[33] = "12345678901234567890123456789012";
char GAME_ID[33] = "12345678901234567890123456789012";

int blockCount = 0, transactionCount = 0;
vector<string> hostList;
vector<Transaction> wallet;

using namespace std;

int main(int argc, char **argv)
{
	mkdir("chain", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	mkdir("chain/blocks", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//chdir("cd ~/.blockchain");
	
	(void) signal(SIGCHLD, reaper);
	
	//create a generic block
	//all will generate this block
	//clear all before restart
	Block block;
	block.setTempValue(true);
	string s = "chain/blocks/block" + block.getTitle();
	mkdir(s.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	saveBlockToFile(block);
	
	//host array will be initialized then never touched again
	//assume hosts file is initialized
	ifstream hosts;
	hosts.open("chain/hosts");
	string temp;
	if (!hosts.good())
	{
		exit(1);
	}
	while(getline(hosts, temp))
	{
		hostList.push_back(temp);
	}
	hosts.close();
	
	ifstream id;
	id.open("chain/id");
	if (!id.good())
	{
		exit(1);
	}
	getline(id, temp);
	string user_id = temp;
	id.close();
	
	int pid;
	if (argc == 2 && argv[1] == "publisher")
	{
		pid = fork();
		// create a fork to create new blocks
		if (pid == 0)
		{
			//create a block every ten minutes or 30 secs
			while (1)
			{
				sleep(30);
				loadBlockCount();
				// if it is this nodes turn, make only 1 publisher
				// create and broadcast block, then save upon approval;
				string s = "chain/blocks/block" + block.getTitle() + "/" + block.getTitle();
				Block newBlock(blockCount + 1, sha256_file(s));
				updateBlockCount(blockCount + 1);
				s = "chain/blocks/block" + newBlock.getTitle();
				mkdir(s.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				broadcastBlock(block, BLOCK_PORT, hostList);
				// generate 10 keys
				updateWallet(publishTransactions(user_id, PUBLISHER_ID, GAME_ID, 10, TRANSACTION_PORT, hostList));
			}
			exit(0);
		}
	}
	
	pid = fork();
	
	if (pid == 0)
	{
		server();
	}
	else if (pid > 0)
	{
		// parent ui and client
		// design a user interface
		// only stdout from here
		while (1)
		{
			//request the current chain settings and update accordingly from a host
			sleep(3);
			loadBlockCount();
			loadTransactionCount();
			userInterface(cout, blockCount, transactionCount);
			loadWallet();
			client(hostList, wallet, TRANSACTION_PORT);
			updateWallet(wallet);
		}
	}
	else
	{
		cerr << "Unable to open fork\n";
		exit(EXIT_FAILURE);
	}
	//system ("rm -rf chain/blocks");
	return 0;
}
