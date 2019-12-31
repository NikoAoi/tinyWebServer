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

//��������������������ͻ���socketid 
void* request_handler(void*); 

//��������������������ͻ���socketid����������         
void error_handler(int, int); 

//����ֱ�ӷ����ļ���Դ������������ͻ���socketid���ļ��� 
void response_file(int, const char*); 

//��ʼ��web������ 
int initialize_server(); 

//��ȡ�����һ�У���һ�������ǿͻ���socketid���ڶ��������Ǵ��������ַ������� 
int get_request_line(int, char*);

//������󣬴���������ͻ���socketid 
void fresh_request(int); 

//��������ִ��cgi����Ļ��� 
void execute_cgi(int, const char *,const char *,const char *);

//����cgi����ܵ�����һ������������ܵ�ָ�룬�ڶ�������������ܵ�ָ�� 
void create_cgi_pipe(int, int *, int *); 

//�ر����е�cgi�ܵ� 
void close_all_cgi_pipe(int *, int *);
