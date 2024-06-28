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

#define SIZE 128

void *sendfun(void *args)
{
	struct sockaddr_in caddr;
	caddr.sin_family = AF_INET;
	caddr.sin_port = 1235;
	caddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int ss = *((int*) args);
	char data[SIZE];
	
	while(1)
	{
		fgets(data, SIZE, stdin);
		printf("\n");
		sendto(ss, (void*) data, strlen(data), 0, (struct sockaddr*) &caddr, sizeof(caddr));

		if(strcmp(data,"bye\n") == 0)
			break;
	}

	printf("Sending thread closed\n");
	pthread_exit(NULL);
}

void *recvfun(void *args)
{
	int ss = *((int*) args), n, i, j;
	char data[SIZE];
	while(1)
	{
		n = recvfrom(ss, (void*) data, SIZE, 0, NULL, NULL);
		data[n-1] = '\0';
		
		for(i = 0; i < n; ++i) {
			printf("\t\t");
			for(j = i; j < i+40 && j < n; ++j)
				printf("%c", data[j]);
			printf("\n");
			i = j-1;
		}
		printf("\n");

		if(strcmp(data,"bye") == 0)
			break;
	}
	printf("\t\tReceiving thread closed\n");
	pthread_exit(NULL);
}

int main()
{
	// creating server socket
	int ss = socket(AF_INET, SOCK_DGRAM, 0);

	// creating object socket address for server
	struct sockaddr_in saddr;
	//reuse
	int opt = 1;
	setsockopt(ss,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	saddr.sin_family = AF_INET;
	saddr.sin_port = 1234;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// binding server socker
	bind(ss, (struct sockaddr*) &saddr, sizeof(saddr));

	pthread_t snd, rec;

	pthread_create(&snd, NULL, sendfun, (void*) &ss);
	pthread_create(&rec, NULL, recvfun, (void*) &ss);

	pthread_join(snd, NULL);
	pthread_join(rec, NULL);

	printf("\nChat closed\n");

	close(ss);
}
