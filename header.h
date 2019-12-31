#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define isSpace(x) isspace((int)(x))

#define PAGE_NOT_FOUND 1
#define CGI_ERROR 2
#define REQUEST_ERROR 3
#define REQUEST_METHOD_ERROR 4

#define LISTEN_PORT 8089 

#define SERVER_INFO "Server: tiny_web_server/1.1.0\r\n"

//请求处理器，传入参数：客户端socketid 
void* request_handler(void*); 

//错误处理器，传入参数：客户端socketid，错误类型         
void error_handler(int, int); 

//用于直接返回文件资源，传入参数：客户端socketid，文件名 
void response_file(int, const char*); 

//初始化web服务器 
int initialize_server(); 

//获取请求的一行，第一个参数是客户端socketid，第二个参数是存放请求的字符串数组 
int get_request_line(int, char*);

//清空请求，传入参数：客户端socketid 
void fresh_request(int); 

//用于设置执行cgi程序的环境 
void execute_cgi(int, const char *,const char *,const char *);

//建立cgi程序管道，第一个参数是输入管道指针，第二个参数是输出管道指针 
void create_cgi_pipe(int, int *, int *); 

//关闭所有的cgi管道 
void close_all_cgi_pipe(int *, int *);
