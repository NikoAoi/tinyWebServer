#include "header.h" 
void *request_handler(void* re){//用于处理请求
	int request = *((int*)re);
	int m=0,n=0,pipe=0,num;
	char method[512],buf[1024],link[512],path[512];
	char *request_str=NULL;
	struct stat sta;
	num=get_request_line(request,buf,sizeof(buf));
	while(!isSpace(buf[n])&&(m<sizeof(method)-1)){
		method[m]=buf[n];//将请求方法存到method中
		m++;
		n++;
	}
	method[m]='\0';
	if(strcasecmp(method,"POST")&&strcasecmp(method,"GET")){
		//请求不是GET或POST则返回
		error_handler(request,REQUEST_METHOD_ERROR);
		return (void*)NULL;
	}
	if(strcasecmp(method,"POST")==0){
		//当请求为POST时开启pipe
		pipe=1;
	}
	while(isSpace(buf[n])&&(n<sizeof(buf))){
		n++;
	}
	m=0;
	while(!isSpace(buf[n])&&(m<sizeof(link)-1)&&(n<sizeof(buf))){
		link[m]=buf[n];//记录下link
		m++;
		n++;
	}
	link[m]='\0';
	
	if(strcasecmp(method,"GET")==0){
		request_str=link;
		while((*request_str!='?')&&(*request_str!='\0')){
			request_str++;//移动至?或、\0处
		}
		if((*request_str=='?')){
			pipe=1;//当到?时开启pipe
			*request_str='\0';
			request_str++;
		}
	}
	sprintf(path,"resources%s",link);
	if(path[strlen(path)-1]=='/'){
		strcat(path,"index.html");//拼接路径
	}
	if(stat(path,&sta)==-1){
		while((num>0)&&strcmp("\n",buf)){
			num=get_request_line(request,buf,sizeof(buf));
		}
		error_handler(request,PAGE_NOT_FOUND);
	}else{
		if((sta.st_mode&S_IFMT)==S_IFDIR){
			strcat(path,"index.html");
		}
		if((sta.st_mode&S_IXUSR)||(sta.st_mode&S_IXGRP)||(sta.st_mode&S_IXOTH)){
			pipe=1;
		}
		if(!pipe){
			response_file(request,path);
		}else{
			execute_cgi(request,method,request_str,path);
		}
	}
	close(request);
} 
void error_handler(int request, int error_type){//错误处理函数
	char buf[1024];
	if(error_type==PAGE_NOT_FOUND){//页面找不到,404
		sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf,SERVER_INFO);
		send(request,buf,strlen(buf),0);
		sprintf(buf, "Content-Type: text/html\r\n\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf, "<HTML><TITLE>404 Not Found</TITLE>\r\n");  
		send(request,buf,strlen(buf),0);
		sprintf(buf, "<BODY><h1>404 NOT FOUND</h1><P>Unable to access this site\r\nBecause the our's tinyWebServer can not work correctly\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf,"or the resource you requested is invalid or nonexistent\r\n</BODY></HTML>\r\n");
		send(request,buf,strlen(buf),0);
	}else if(error_type==CGI_ERROR){//不能正常执行，CGI_ERROR
		sprintf(buf,"HTTP/1.0 500 Internal Server Error\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf, "Content-Type: text/html\r\n\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf, "<P>CGI execution error.\r\n");
		send(request,buf,strlen(buf),0);
	}else if(error_type==REQUEST_ERROR){//语义有误，服务器无法理解，REQUEST_ERROR
		sprintf(buf,"HTTP/1.0 400 REQUEST ERROR\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf, "Content-Type: text/html\r\n\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf, "<P>Request error,The current request is not understood by the Server.\r\n");  
		send(request,buf,strlen(buf),0);
	}else if(error_type==REQUEST_METHOD_ERROR){//请求方法有误，服务器无法识别，REQUEST_METHOD_ERROR
		sprintf(buf, "HTTP/1.0 501 Not Implemented\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf,SERVER_INFO);
		send(request,buf,strlen(buf),0);
		sprintf(buf, "Content-Type: text/html\r\n\r\n");
		send(request,buf,strlen(buf),0);
		sprintf(buf, "<HTML><TITLE>Method Not Implemented</TITLE>\r\n");  
		send(request,buf,strlen(buf),0);
		sprintf(buf, "<P>The request method is incorrect and the server cannot recognize it.\r\n</HTML>");  
		send(request,buf,strlen(buf),0);
	}else{
		sprintf(buf, "<P>The error_type is wrong.");
		send(request,buf,strlen(buf),0);
	}
}
void response_file(int request,const char* filename){
	char buf[1024], buf1[1024], buf2[1024];
	int num=1;
	FILE *file=NULL;
	buf[0]='A';
	buf[1]='\0';
	while((num>0)&&strcmp("\n",buf)){
		num=get_request_line(request,buf,sizeof(buf));
	}
	file=fopen(filename,"r");
	if(file==NULL)	error_handler(request,PAGE_NOT_FOUND);
	else{
		//sprintf(buf,"");
		strcpy(buf1,"HTTP/1.0 200 OK\r\n");//200请求成功
		send(request,buf1,strlen(buf1),0);
		sprintf(buf1,SERVER_INFO);
		send(request,buf1,strlen(buf1),0);
		sprintf(buf1, "Content-Type: text/html\r\n\r\n");
		send(request,buf1,strlen(buf1),0);
		//sprintf(buf1,"");
		fgets(buf2,sizeof(buf2),file);
		while(!feof(file)){
			send(request,buf2,strlen(buf2),0);
			fgets(buf2,sizeof(buf2),file);
		}
	}
	fclose(file);
}
