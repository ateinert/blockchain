//main.cpp

//sudo apt install libssl-dev
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "blockchain.h"

int main(int argc, char **argv)
{
	//chdir("cd ~/.blockchain");
	if (argc < 3)
	{
		exit(1);	
	}
	
	char *service = argv[1];
	const int numHosts = argc - 2;
	char *hosts[numHosts];
	int i;
	for (i = 0; i < numHosts)
	{
		hosts[i] = argv[i + 2];
	}
	
	(void) signal(SIGCHLD, reaper);
	pid = fork();
	
	if (pid == 0)
	{
		//child server
		server();
	}
	else if (pid > 0)
	{
		// parent ui and client
		// design a user interface
		// only stdout from here
		while (1)
		{
			int blockcount = 0;
			int transactioncount = 0;
			printf("Blocks sent: %d\n", blockcount);
			printf("Transactions sent: %d\n", transactioncount);
			printf("Press ENTER to send a block and 10 transactions");
			getchar();
			char buffer[65];
			sha256_file(itoa(blockcount), buffer);
			Block block = createBlock(blockcount, buffer);
			blockcount++;
			broadcastBlock(block, hosts, service, numHosts);
		}
	}
	else
	{
		
		exit(EXIT_FAILURE);
	}
	//system ("rm -rf chain/blocks");
	return 0;
}
