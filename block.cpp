//block.cpp

#include "blockchain.h"
#include <string.h>
#include <string>

Block::Block()
{
	temp = false;
	prevHash[0] = '0';
	prevHash[1] = '\0';
	blockTitle[0] = '0';
	blockTitle[1] = '\0';
	blockNumber = 0;
	creationTime = 0;
}

Block::Block(int num, string hash)
{
	temp = false;
	strcpy(prevHash, hash.c_str());
	strcpy(blockTitle, to_string(num).c_str());
	blockNumber = num;
	creationTime = time(0);
}

bool Block::validate()
{
	if (blockNumber < 0)
		return false;
	if (strcmp(to_string(blockNumber).c_str(), blockTitle) != 0)
		return false;
	
	return true;
}

void Block::setTempValue(bool val)
{
	temp = val;
}

string Block::getPrevHash()
{
	return string(prevHash);
}

string Block::getTitle()
{
	return string(blockTitle);
}

long long int Block::getNumber()
{
	return blockNumber;
}

time_t Block::getCreationTime()
{
	return creationTime;
}
