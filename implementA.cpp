#include "header.h" 
void create_cgi_pipe(int clientfd, int* pipe_in, int* pipe_out){
	if (pipe(pipe_in) < 0) {
		error_handler(clientfd, CGI_ERROR);
		return;
	}
	if (pipe(pipe_out) < 0) {
		error_handler(clientfd, CGI_ERROR);
		return;
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
	while(get_request_line(clientfd, buffer, sizeof(buffer)) > 0 && strcmp(buffer, "\n") != 0);
}

void execute_cgi(int clientfd, const char *request_method, const char *query_string, const char *cgi_path){
	int content_length = -1, pipe_in[2], pipe_out[2];
	int isGet = 0;
	char buffer[2048];
	pid_t pid;
	//if(request_method != NULL) printf("method: %s\n", request_method);
        //if(query_string != NULL) printf("string: %s\n", query_string);
        //if(cgi_path != NULL) printf("path: %s\n", cgi_path);

	if(strcasecmp(request_method, "GET") == 0) isGet = 1;
	if(isGet) fresh_request(clientfd);
	else{
		while(get_request_line(clientfd, buffer, sizeof(buffer)) > 0 && strcmp(buffer, "\n") != 0){
			buffer[15] = '\0';
			if(!strcasecmp("content-length:", buffer)) content_length = atoi(&buffer[16]);
		}
		if(content_length == -1) {
			error_handler(clientfd, REQUEST_ERROR);
			return;
		}
	}
	sprintf(buffer, "HTTP/1.0 200 OK\r\n");
	send(clientfd, buffer, strlen(buffer), 0);
	create_cgi_pipe(clientfd, pipe_in, pipe_out);
	//printf("in: %d  %d  out: %d %d\n", pipe_in[0], pipe_in[1], pipe_out[0], pipe_out[1]);
	//printf("content_length:%d method: %s, path: %s\n", content_length, request_method, cgi_path);
	if((pid = fork()) != 0){
		char ch;
		int status;
		//printf("1:content_length:%d method: %s, path: %s\n", content_length, request_method, cgi_path);
		if(!isGet){
			for(int i = 0; i < content_length; i++){
				recv(clientfd, &ch, 1, 0);
				//printf("%c\n",ch);
				write(pipe_in[1], &ch, 1);
			}
		}
		//printf("2: content_length:%d method: %s, path: %s\n", content_length, request_method, cgi_path);
		while (read(pipe_out[0], &ch, 1) > 0) {
			//printf("%c",ch);
			send(clientfd, &ch, 1, 0);
		}
		//printf("3: content_length:%d method: %s, path: %s\n", content_length, request_method, cgi_path);
		close_all_cgi_pipe(pipe_in, pipe_out);
		waitpid(pid, &status, 0);
	}
	else{
		char query[255], method[255], length[255];
		dup2(pipe_in[0], 0);
		dup2(pipe_out[1], 1);
		sprintf(method, "REQUEST_METHOD=%s", request_method);
		putenv(method);
		if(isGet){
			sprintf(query, "QUERY_STRING=%s", query_string);
			putenv(query);
		}
		else{
			sprintf(length, "CONTENT_LENGTH=%d", content_length);
			putenv(length);
		}
		//printf("%s %s\n", method, length);
		close(pipe_in[1]);
		close(pipe_out[0]);
		//iprintf("content_length:%d method: %s, path: %s\n", content_length, request_method, cgi_path);
		execl(cgi_path, cgi_path, NULL);
		//printf("4: content_length:%d method: %s, path: %s\n", content_length, request_method, cgi_path);
		return;
	}
} 

//���ڳ�ʼ��web������ 
int initialize_server(u_short* port){
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
	listen_addr.sin_port = htons(htons(*port));
	if(bind(serverfd, (struct sockaddr*)&listen_addr, sizeof(listen_addr)) < 0){
		fprintf(stderr, "bind error!\n");
		exit(2);
	}
	int len = sizeof(listen_addr);
	getsockname(serverfd, (struct sockaddr *)&listen_addr, (socklen_t*)&len);
        *port = ntohs(listen_addr.sin_port);
	listen(serverfd, 5);
	return serverfd;
}

//���ڻ�ȡ�����һ�� 
int get_request_line(int sock, char* buf, int size){
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';
    return(i);
}


