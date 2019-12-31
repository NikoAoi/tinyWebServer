#include "header.h" 
void create_cgi_pipe(int clientfd, int* pipe_in, int* pipe_out){
	if (pipe(pipe_in) < 0) {
		error_handler(clientfd, CGI_ERROR);
		exit(1);
	}
	if (pipe(pipe_out) < 0) {
		error_handler(clientfd, CGI_ERROR);
		exit(1);
	}
}

void close_all_cgi_pipe(int *pipe_in, int *pipe_out){
	close(pipe_in[0]);
	close(pipe_in[1]);
	close(pipe_out[0]);
	close(pipe_out[1]);
}

void fresh_request(int clientfd){
	char buffer[2048];
	while((get_request_line(clientfd, buffer)) > 0 && strcmp(buffer, "\n") != 0);
}

void execute_cgi(int clientfd, const char *request_method, const char *query_string, const char *cgi_path){
	int content_length = -1, pipe_in[2], pipe_out[2];
	int isGet = 0;
	char buffer[2048], query[1024], method[1024], length[1024];
	pid_t pid;
	if(strcasecmp(request_method, "GET") == 0) isGet = 1;
	if(isGet) fresh_request(clientfd);
	else{
		while((get_request_line(clientfd, buffer)) > 0 && strcmp(buffer, "\n") != 0){
			buffer[15] = '\0';
			if(!strcasecmp("content-length:", buffer)) content_length = atoi(&buffer[16]);
		}
		if(content_length == -1) {
			error_handler(clientfd, REQUEST_ERROR);
			exit(1);
		}
	}
	sprintf(buffer, "HTTP/1.0 200 OK\r\n");
	send(clientfd, buffer, strlen(buffer), 0);
	create_cgi_pipe(clientfd, pipe_in, pipe_out);
	if((pid = fork()) != 0){
		char ch;
		int status;
		if(!isGet){
			for(int i = 0; i < content_length; i++){
				recv(clientfd, &ch, 1, 0);
				write(pipe_in[1], &ch, 1);
			}
		}
		while (read(pipe_out[0], &ch, 1) > 0) send(clientfd, &ch, 1, 0);
		close_all_cgi_pipe(pipe_in, pipe_out);
		waitpid(pid, &status, 0);
	}
	else{
		dup2(pipe_in[0], 0);
		dup2(pipe_out[1], 1);
		if(isGet){
			sprintf(query, "QUERY_STRING=%s", query_string);
			putenv(query);
		}
		else{
			sprintf(length, "CONTENT_LENGTH=%d", content_length);
			putenv(length);
		}
		sprintf(method, "REQUEST_METHOD=%s", request_method);
		putenv(method);
		close(pipe_in[1]);
		close(pipe_out[0]);
		execl(cgi_path, cgi_path, NULL);
		exit(0);
	}
} 

//用于初始化web服务器 
int initialize_server(){
	int serverfd;
	struct sockaddr_in listen_addr;
	serverfd = socket(PF_INET, SOCK_STREAM, 0);
	if(serverfd < 0){
		fprintf(stderr, "get socketfd error!\n");
		exit(1);
	}
	memset(&listen_addr, 0, sizeof(struct sockaddr_in));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(LISTEN_PORT);
	if(bind(serverfd, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0){
		fprintf(stderr, "bind error!\n");
		exit(2);
	}
	listen(serverfd, 5);
	return serverfd;
}

//用于获取请求的一行 
int get_request_line(int socketfd, char* buf){
	int buf_size = sizeof(buf), num, i;
	char ch = '\0';
	for(i = 0; i < buf_size && ch != '\n'; i++){
		num = recv(socketfd, &ch, 1, 0);
		if(num < 0) fprintf(stderr, "socket receive error!\n");
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
	return i;
}


