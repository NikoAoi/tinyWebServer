#include "header.h" 
void request_handler(int request){//用于处理请求
	int m=0,n=0,pipe=0,num;
	char method[512],buf[1024],link[512],path[512];
	char *request_str=NULL;
	struct stat sta;
	num=get_request_line(request,buf);
	while(!isSpace(buf[n])&&(m<sizeof(method)-1)){
		method[m]=buf[n];//将请求方法存到method中
		m++;
		n++;
	}
	method[m]='\0';
	if(strcasecmp(method,"POST")&&strcasecmp(method,"GET")){
		//请求不是GET或POST则返回
		unimplement(request);
		return;
	}
	if(strcasecmp(method,"POST")==0){
		//当请求为POST时开启pipe
		pipe=1;
	}
	while(isSpace(buf[n])&&(n<sizeof(buf))){
		n++
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
	sprintf(path,"htdocs%s",link);
	if(path[strlen(path)-1]=='/'){
		strcat(path,"index.html");//拼接路径
	}
	if(stat(path,&sta)==-1){
		while((num>0)&&strcmp("\n",buf)){
			num=get_request_line(request,buf);
		}
		not_found(request);//回应找不到
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
			execute_cgi(request,path,method,request_str);
		}
	}
	close(request);
} 