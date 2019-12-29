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
int get_request_line(int sock, char* buf){
	int buf_size = sizeof(buf);
	char ch = '\0';
	for(int i = 0; i < buf_size && ch != '\n'; i++){
		n = recv(sock, &ch, 1, 0);
		if(n > 0){
			if(ch == '\r'){
				n = recv(sock, &ch, 1, MSG_PEEK);
                if ((n > 0) && (ch == '\n')) recv(sock, &ch, 1, 0);
				else ch = '\n';
				buf[i] = ch;
			}
		}
		else ch = '\n';
	} 
	buf[i] = '\0';
    return(i);
}

int main(void){
	
}

