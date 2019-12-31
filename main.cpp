#include "header.h" 
int main(void){
	int serverfd, clientfd;
	pthread_t t1;
	struct sockaddr_in client_addr;
	serverfd = initialize_server();
	while(1){
		clientfd = accept(serverfd, (struct sockaddr *)&client_addr, &(sizeof(client_addr));
		pthread_create(&t1 , NULL, request_handler, (void*)&clientfd);
	}
	close(serverfd);
    return 0;
}
