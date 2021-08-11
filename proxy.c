/**********************************************************/
/* Bethelhem Nigat NIbret
/* 20170884
/* Computer Network - Project One
/* Socket Programming
/**********************************************************/
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#define MAXLINE 2048
#define MAX_LEN 2048

#define BUF_SIZE 2048


#define MAX_STRING_LEN (1000000)

void DEBUG (char * message) {
    //printf("%s\n", message);
    fflush(NULL);
    return;
}



void proxy(int conn_fd, char * blacklist)
{
    char method[BUF_SIZE]; 
    char url[BUF_SIZE]; 
    char http_version[BUF_SIZE]; 
    char host[BUF_SIZE]; 
    char host_name[BUF_SIZE];
    //char* host_name1; 
    char host_header[BUF_SIZE];
    char port_number[BUF_SIZE];
    char path[BUF_SIZE]; 
    char * temp;
    char * temp1;
    char * temp2;
    char forward_request[BUF_SIZE];  
    char* save_ptr; 
    char save_url[BUF_SIZE];
    char buf[BUF_SIZE]; 
    char buf_temp[BUF_SIZE];
    char buffer[BUF_SIZE];


    int len=0;
    int total_len=0;

    while(1) {
        if((len = recv(conn_fd, buf_temp, BUF_SIZE,0))==-1)
        {
            perror("recieve error:"); 
            exit(0); 
        }

        if(len>0)
        {
            strncpy(buf+total_len, buf_temp, len);
            total_len+= len;
        }

        if(strstr(buf, "\r\n\r\n")!=NULL) break; 
        bzero(buf_temp, BUF_SIZE);
    }

    //first token should be GET
    temp= strtok_r(buf, " ",&save_ptr);
    if (temp==NULL)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }
    strcpy(method, temp);

    if (strcmp(method, "GET")!=0)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }

    temp= strtok_r(NULL, " ",&save_ptr); //hostname, port, path 
    if (temp==NULL)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }
    strcpy(url, temp);
    if (strncmp(url, "http://", 7)!=0)
    {
        send(conn_fd, "400 Bad Request Error",strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }

    //check http validty 
    temp= strtok_r(NULL, "\r\n",&save_ptr);
    if (temp==NULL)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }
    strcpy(http_version, temp);
    if (strncmp(http_version, "HTTP/1.0", 8)!=0)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }


    temp1= strstr(save_ptr, "Host: ");
    if(temp1==NULL)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }

    temp1= strtok_r(temp1+6, "\r\n", &save_ptr);
    if (temp1==NULL)
    {
        send(conn_fd, "400 Bad Request Error",strlen("400 Bad Request Error"),0 );
        close(conn_fd);
        exit(0);
    }

    strcpy(host_name, temp1); 

    char * temp_url;
    char host_name_url[BUF_SIZE];

    if ((temp_url = strstr(url+7,":" ))!=NULL)
    {
        strncpy(host_name_url, url+7, temp_url-url-7);
    }
    else if ((temp_url = strstr(url+7,"/" ))!=NULL)
    {
        strncpy(host_name_url, url+7, temp_url-url-7);
    }
    else
    {
        strncpy(host_name_url, url+7, strlen(url)-7);   
    }
    //printf("host_name_url: |%s|\n", host_name_url);

    ///// HTTP REQUEST IS VALID ///////////
    //case 1, there is port number
    temp1= strstr(url+7,":" );
    //printf("temp1 |%s| \n", temp1);
    if (temp1==NULL)
    {
        strcpy(port_number,"80");
    }
    else
    {
        temp2= strstr(temp1+1, "/");
        if (temp2==NULL) // there is no slash 
            strncpy(port_number,temp1+1, strlen(temp1+1)); 
        else
        {
            strncpy(port_number,temp1+1, temp2-temp1-1); 
        }
    }
    //printf("port: %s\n", port_number);
    strcpy(save_url, url); 
    if (strcmp(host_name, host_name_url)!=0)
    {
        send(conn_fd, "400 Bad Request Error", strlen("400 Bad Request Error"),0);
        close(conn_fd);
        exit(0);
    }
        
    //case 2 there is path name
    //use save_url-> http::www.com:3030/content/web.p"

    temp1= strchr(save_url, '/');//www.com/content
    //printf("save_url: %s\n", temp1+2);
    temp2= strchr(temp1+2, '/'); // /content/ss
    if (temp2==NULL) // no path name
    {
        sprintf(forward_request, "GET / HTTP/1.0\r\nHost: %s\r\n\r\n", host_name);
    }

    else {
        strcpy(path,temp2);
        sprintf(forward_request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", path, host_name);
    }

    //printf("forward_request |%s|\n", forward_request );
    /////send the request to the server

    //create a connection with the server

    struct sockaddr_in server_addr2;
    struct in_addr * server_ip;
    struct in_addr addr2;
    struct hostent * hostp;
    int port2; 
    port2= atoi(port_number);

    int client_fd2;
    char url_temp[BUF_SIZE];

    for(int j=0; j< strlen(host_name); j++)
    {
        url_temp[j] = tolower(host_name[j]);
    }
    if (strstr(blacklist, url_temp)!=NULL)
    {
        bzero(host_name, BUF_SIZE);
        memcpy(host_name, "warning.or.kr", strlen("warning.or.kr"));
        bzero(path, BUF_SIZE);
        memcpy(path, "/", 1);
    }

    if (inet_aton(host_name, &addr2) != 0) 
    {
        hostp = gethostbyaddr((const char *)&addr2, sizeof(addr2), AF_INET);
    }
    else 
    {
        hostp = gethostbyname(host_name);
    }
    if (hostp == NULL) 
    {
        send(conn_fd,"503 Service Unavailable",strlen("503 Service Unavailable"),0);
        close(conn_fd);
        _exit(0);
    }

    //printf("Host: |%s|\nPort: |%d|", host_name, port2);
    server_ip= (struct in_addr*) hostp->h_addr; 
    bzero(&server_addr2, sizeof(server_addr2));

    server_addr2.sin_family = AF_INET;
    server_addr2.sin_port = htons(port2);
    server_addr2.sin_addr = *server_ip;
    bzero(&(server_addr2.sin_zero), sizeof(server_addr2.sin_zero));

    if((client_fd2 = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket: ");
        _exit(-1);
    }
    DEBUG ("socket created");

    if((connect(client_fd2, (struct sockaddr *) &server_addr2, sizeof(struct sockaddr))) == -1)
    {
        perror("connect: ");
        _exit(-1);
    }
        DEBUG ("Connection created");

    if (send(client_fd2, forward_request, strlen(forward_request), 0)==-1)
    {
        perror("send");
    }
    else
    {
         DEBUG("2");
        int n;
        bzero(buffer, MAX_LEN);
        while((n = recv(client_fd2, buffer, MAX_LEN, 0)) > 0)
        {
            DEBUG("3");
            if (send(conn_fd, buffer, n, 0)==-1)
            {
                DEBUG("4");
                perror("send:");
            }
            DEBUG("5");
            bzero(buffer, MAX_LEN);
        }
        if (n==-1)
        {
            perror("recv:");
        }
    }
    DEBUG("END");
    _exit(0);       
}



int main(int argc, char**argv)
{
    if (argc!=2)
    {
        fprintf(stderr, "wrong arguments:\n");
        exit(EXIT_FAILURE);
    }

    int port= atoi(argv[1]); 
    if(port<0 || port > 65536)
    {
        fprintf(stderr, "port number should be between 0 and 65536\n");
        exit(EXIT_FAILURE);
    }

    int listen_fd, conn_fd;
    struct sockaddr_in server_addr, client_addr;
    int addr_len;
    int client_len;
    pid_t childpid;
    int bytes; 
    char buf[BUF_SIZE];
    char buffer[BUF_SIZE];

    /****************************************************************************/
    char * blacklist = malloc(MAX_STRING_LEN);
    char temp_buffer[MAX_LEN];
    if(!isatty(STDIN_FILENO))
    {
        int n = 0;
        while(fgets(temp_buffer, MAX_LEN, stdin)!=NULL)
        {
            for(int i = 0; i < strlen(temp_buffer); i++){
               temp_buffer[i] = tolower(temp_buffer[i]);
            }
            strncpy(blacklist + n, temp_buffer, strlen(temp_buffer));
            n += (strlen(temp_buffer));
            bzero(temp_buffer, MAX_LEN);
        }
    }
    //printf("%s\n",blacklist );
    /****************************************************************************/
 

    if ((listen_fd = socket(AF_INET, SOCK_STREAM,0)) == 0)
    {
        perror("socket: ");
        exit(EXIT_FAILURE);
    }

       bzero(&server_addr, sizeof(server_addr));
    bzero(&client_addr, sizeof(client_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        perror("bind: ");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 100000) != 0 )
    {
        perror("listen: ");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        client_len = sizeof(client_addr);
        conn_fd = accept(listen_fd, (struct sockaddr*)&client_addr, (socklen_t *)&client_len);
        if (conn_fd==-1)
        {
            perror("accept error"); 
        }

        if ((childpid = fork()) == 0)
        {
            close(listen_fd); 
            proxy(conn_fd, blacklist);
            close(conn_fd);
            exit(0);
        }
        else if (childpid == -1)
        {
           perror("fork ");
           exit(EXIT_FAILURE); 
        }

        close(conn_fd); 

    }
    return 0; 
 }

