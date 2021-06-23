#include <sys/types.h>    
#include <sys/socket.h>    
#include <time.h>        
#include <errno.h>
#include <fcntl.h>        
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>    
#include <sys/uio.h>       
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>        
#include <sys/select.h>    
#include <string.h>  
#include <arpa/inet.h>  

static int count;
const int MAXLINE = 4096;

static void sig_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main(int argc, char **argv) {
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9999);

    int rt1 = bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        printf("bind failed\n");
        return 0;
    }

    int rt2 = listen(listenfd, 5);
    if (rt2 < 0) {
        printf("listen failed \n");
        return 0;
    }

    signal(SIGINT, sig_int);
    signal(SIGPIPE, SIG_DFL);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
        printf("bind failed \n");
        return 0;
    }

    char message[MAXLINE];
    count = 0;

    for (;;) {
        int n = read(connfd, message, MAXLINE);
        if (n < 0) {
            printf("error read\n");
            return 0;
        } else if (n == 0) {
            printf("client closed \n");
            return 0;
        }
        message[n] = 0;
        printf("received %d bytes: %s\n", n, message);
        count++;

        char send_line[MAXLINE + 10];
        sprintf(send_line, "Hi, %s", message);

        sleep(5);

        int write_nc = send(connfd, send_line, strlen(send_line), 0);
        printf("send bytes: %u \n", write_nc);
        if (write_nc < 0) {
            printf("error write\n");
            return 0;
        }
    }
    return 0;
}
