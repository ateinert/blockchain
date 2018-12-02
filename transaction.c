//transaction.cpp

#include "blockchain.h"
#include <string.h>
#include <stdlib.h>

Transaction::Transaction()
{
	transactionCount = 0;
	temp = false;
	strcpy(prevHash,"0");
	strcpy(ownerKey, "0");
	strcpy(license, "0");
	creationTime = 0;
}

Transaction::Transaction(int count, string hash, string owner, string key)
{
	transactionCount = count;
	temp = false;
	strcpy(prevHash,hash.c_str());
	strcpy(ownerKey,owner.c_str());
	strcpy(license,key.c_str());
	creationTime = time(0);
}

bool Transaction::validate()
{
	return true;
}

void Transaction::setTempValue(bool val)
{
	temp = val;
}

string Transaction::getPrevHash()
{
	return string(prevHash);
}

string Transaction::getOwner()
{
	return string(ownerKey);
}

string Transaction::getLicense()
{
	return string(license);
}

time_t Transaction::getCreationTime()
{
	return creationTime;
}
