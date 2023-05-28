#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>



int main(){
	int ss = socket(AF_INET,SOCK_STREAM,0);
	perror("socket");
	struct sockaddr_in saddr,caddr;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = 1234;
	saddr.sin_family = AF_INET;
	int opt = 1;
	setsockopt(ss,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	bind(ss,(struct sockaddr*)&saddr,sizeof(saddr));
	perror("bind");
	listen(ss,1);
	int len = sizeof(caddr);
	int ts = accept(ss,(struct sockaddr*)&caddr,&len);
	perror("accept");
	int ack = 1;
	while(1){
		int r;
		read(ts,(void *)&r,sizeof(int));
		printf("%d",r);
		int delay = (rand()%7) + 1;
		if(r <= ack){
			sleep(delay);
		}
		write(ts,&ack,sizeof(ack));
		printf("Sending Acknowledgement for %d\n",r);
		if(ack == r){
			ack = ack + 1;
		}
	}
}
