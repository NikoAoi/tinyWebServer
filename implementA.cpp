#include "header.h" 

//用于初始化web服务器 
void initialize_server(){
	int listenfd;
	struct sockaddr_in listen_addr;

	listenfd = socket(PF_INET, SOCK_STREAM, 0);
	if(listenfd < 0){
		fprintf(stderr, "socket error!\n");
		exit(1);
	}

	memset(&listen_addr, 0, sizeof(struct sockaddr_in));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(LISTEN_PORT);
	if(bind(listenfd, &listen_addr, sizeof(struct sockaddr_in)) < 0){
		fprintf(stderr, "bind error!\n");
		exit(2);
	}

	listen(listenfd, 5);
}

//用于获取请求的一行 
int get_request_line(int socketfd, char* buf){
	int buf_size = sizeof(buf), num, i;
	char ch = '\0';
	for(int= 0; i < buf_size && ch != '\n'; i++){
		num = recv(socketfd, &ch, 1, 0);
		error_handler(socketfd, num);
		if(num == 0) ch = '\n';
		else {
			if(ch == '\r'){
				num = recv(socketfd, &ch, 1, MSG_PEEK);
                if ((num > 0) && (ch == '\n')) recv(socketfd, &ch, 1, 0);
				else ch = '\n';
			}
			buf[i] = ch;
		}
	} 
	buf[i] = '\0';
    return(i);
}

int main(void){
	
}

