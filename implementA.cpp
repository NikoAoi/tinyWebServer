 void initialize_server(){
	int listenfd;
	struct sockaddr_in listen_addr;
	char buffer[1024];

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

