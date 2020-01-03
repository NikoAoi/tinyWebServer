#include "header.h" 
int serverfd;
void closeSocket(int sig){
	printf("close server!");
	close(serverfd);
	exit(0);
}
int main(void){
	u_short port = 0;
	int clientfd;
	pthread_t t1;
	struct sockaddr_in client_addr;
	serverfd = initialize_server(&port);
	printf("Hi!, Your tinyWebServer running on port %d\n", port);
	int length = sizeof(client_addr);
	signal(SIGINT,closeSocket);
	while(1){
		clientfd = accept(serverfd, (struct sockaddr *)&client_addr,(socklen_t*) &length);
		if(clientfd == -1)fprintf(stderr, "accept error!\n"); 
		if(pthread_create(&t1 , NULL, request_handler, (void*)&clientfd) != 0){
			perror("pthread_create");
		}
	}
    	return 0;
}
