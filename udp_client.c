#include <sys/types.h>     
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE 64

int main()
{
	//creating client socket
	int cs = socket(AF_INET, SOCK_DGRAM, 0);

	// creating object socket address for server
	struct sockaddr_in saddr;

	saddr.sin_family = AF_INET;
	saddr.sin_port = 1234;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	char data[SIZE], res[SIZE];

	while(1) {
		printf("Enter dataword: ");
		scanf("%s", data);

		sendto(cs, (void*) data, sizeof(data), 0, (struct sockaddr*) &saddr, sizeof(saddr));

		if(strcmp(data,"exit") == 0) {
			printf("Closing the connection\n");
			break;
		}

		usleep(100000);

		recvfrom(cs, (void*) res, sizeof(res), 0, NULL, NULL);
		printf("Hamming code is %s\n\n", res);
	}
	close(cs);
}
