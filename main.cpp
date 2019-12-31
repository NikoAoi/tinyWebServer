#include "header.h" 
int serverfd;
void closeSocket(int sig){
	printf("close server!");
	close(serverfd);
	exit(0);
}
int main(void){
	int clientfd;
	pthread_t t1;
	struct sockaddr_in client_addr;
	serverfd = initialize_server();
	int length = sizeof(client_addr);
	signal(SIGINT,closeSocket);
	while(1){
		clientfd = accept(serverfd, (struct sockaddr *)&client_addr,(socklen_t*) &length);
		pthread_create(&t1 , NULL, request_handler, (void*)&clientfd);
	}
    	return 0;
}
