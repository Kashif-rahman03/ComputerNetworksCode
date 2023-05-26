#include <sys/types.h>          
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdlib.h>

#define SIZE 64

char *hamming(char* data, int dataBits) {
	//Calculating number of redundant bits required for the hamming code
    int redundantBits = 1;
	while((1 << redundantBits) < dataBits + redundantBits + 1)
		redundantBits++;
	
    // Calculating the total number of bits in the hamming code
    int totalBits = dataBits + redundantBits;
    int nextRedundantBitPos = 1;

    // A character array to store the hamming code
    char *dataword = malloc(sizeof(char) * SIZE);
    strcpy(dataword, "");
    int i, j = dataBits-1;

    // Placing the redundant bits in the correct position but no parity check
    // (default = 0)
    for(i = 0; i < totalBits; ++i)
    {
        if(i == nextRedundantBitPos - 1)
        {
            strcat(dataword, "0");
            nextRedundantBitPos <<= 1;
        }
        else
        {
            strcat(dataword, (data[j] == '0' ? "0" : "1"));
            j--;
        }
    }

    // Calculating the parity of the redundant bits
    // using even parity
    nextRedundantBitPos = 1;
    int parityCount, k;
    for(i = 0; i < redundantBits; ++i)
    {
        parityCount = 0;
        j = nextRedundantBitPos - 1;
        while(j < totalBits)
        {
            for(k = 0; k < nextRedundantBitPos; ++k, ++j)
            {
                if(dataword[j] == '1')
                    parityCount++;
                if(j == totalBits)
                    break;
            }

            for(k = 0; k < nextRedundantBitPos; ++k, ++j)
                if(j == totalBits)
                    break;
        }
        if(parityCount & 1)
            dataword[nextRedundantBitPos - 1] = '1';
        nextRedundantBitPos <<= 1;
    }

    // Reversing the dataword to get the actual hamming code
    i = 0, j = totalBits - 1;
    char temp;
    while(i < j)
    {
        temp = dataword[i];
        dataword[i] = dataword[j];
        dataword[j] = temp;
        i++;
        j--;
    }

	return dataword;
}

int main()
{
	// creating server socket
	int ss = socket(AF_INET, SOCK_DGRAM, 0);

	// creating object socket address for server
	struct sockaddr_in saddr, caddr;
	int len = sizeof(caddr);

	int opt = 1;
	setsockopt(ss,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	saddr.sin_family = AF_INET;
	saddr.sin_port = 1234;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// binding server socker
	bind(ss, (struct sockaddr*) &saddr, sizeof(saddr));

	char data[SIZE];
	char* res;
	
	while(1) {
		recvfrom(ss, (void*) data, sizeof(data), 0, (struct sockaddr*) &caddr, &len);	
		printf("Data received  is %s\n", data);

		if(strcmp(data,"exit") == 0) {
			printf("Closing the connection\n");
			break;
		}

		res = hamming(data, strlen(data));

		sendto(ss, (void*) res, strlen(res)+1, 0, (struct sockaddr*) &caddr, len);
		printf("Hamming code send successfully\n\n");
	}

	close(ss);
}
