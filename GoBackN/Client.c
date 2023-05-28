#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
int seq = 1,count = 0,timedOut = 1;
int N = 4;

void* timer(void* args){
	timedOut = 0;
	sleep(5);
	timedOut = 1;
	pthread_exit(NULL);
}
pthread_t t;
void* sender(void *args){
	int ts = *((int *)args);
	for(int i = 0; i < N; i++){
		int x = seq+i;
		write(ts,&x,sizeof(int));
		printf("Sending %d\n",seq+i);
	}
	timedOut = 0;
	pthread_create(&t,NULL,timer,NULL);
	pthread_join(t,NULL);
	while(1){
		if(timedOut == 0){
			int x = seq + N;
			write(ts,&x,sizeof(int));
			printf("Sending %d\n",seq+N);
			seq++;
			
		}
		else{
			printf("\nAcknowledgement not Received. Timer timed out.\n");
			for(int i = 0; i < N; i++){
				int x = seq+i;
				write(ts,&x,sizeof(int));
				printf("Re-Sending %d\n",seq+i);
			}
		}
		timedOut = 0;
		pthread_create(&t,NULL,timer,NULL);
		pthread_join(t,NULL);
	}
	pthread_exit(NULL);

}

void* receiver(void* args){
	int ts = *((int *)args);
	while(1){
		int r;
		read(ts,&r,sizeof(int));
		if(r == seq){
			printf("Acknowledgement Received: %d\n",r);
			pthread_cancel(t);
			timedOut = 0;
		}
	}
	pthread_exit(NULL);
}

int main(){
	int cs = socket(AF_INET,SOCK_STREAM,0);
	perror("socket");
	struct sockaddr_in saddr,caddr;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = 1234;
	saddr.sin_family = AF_INET;
	int len = sizeof(saddr);
	connect(cs,(struct sockaddr*)&saddr,len);
	perror("connect");
	int ack = 1;
	pthread_t sen,recv;
	pthread_create(&sen,NULL,sender,&cs);
	pthread_create(&recv,NULL,receiver,&cs);
	pthread_join(sen,NULL);
	pthread_join(recv,NULL);
}
