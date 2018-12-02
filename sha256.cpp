//sha256.cpp
#include <openssl/sha.h>
#include <netinet/in.h>

string sha256(string str)
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), strlen(str.c_str()));
	SHA256_Final(hash, &sha256);
	string s(hash);
	return s;
}

string sha256_file(string path)
{
	FILE *file = fopen(path.c_str(), "rb");
	if(!file) exit(EXIT_FAILURE);

	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	char *buffer = (char*)malloc(bufSize);
	int bytesRead = 0;
	if(!buffer) return ENOMEM;
	while((bytesRead = fread(buffer, 1, bufSize, file)))
	{
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);
	string s(hash)
	fclose(file);
	free(buffer);
	return s;
}