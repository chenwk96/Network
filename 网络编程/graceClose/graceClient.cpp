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

const int MAXLINE = 4096;

int main(int argc, char **argv) {
    int port = 9999;
    int socket_fd; 
    socket_fd = socket(AF_INET, SOCK_STREAM, 0); 
    sockaddr_in server_addr; 
    bzero(&server_addr, sizeof(server_addr)); 
    
    server_addr.sin_family = AF_INET; 
    server_addr.sin_port = htons(port); 
    inet_pton(AF_INET,"127.0.0.1", &server_addr.sin_addr);
    
    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if (connect_rt < 0) {
        printf("wrong!\n");
        return 0;
    }
    char send_line[MAXLINE], recv_line[MAXLINE + 1];
    int n;

    fd_set readmask;
    fd_set allreads;

    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);
    for (;;) {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if (rc <= 0)
            printf("select failed\n");
        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0) {
                printf("read error");
                return 0;
            } else if (n == 0) {
                printf("server terminated \n");
                return 0;
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }
        if (FD_ISSET(0, &readmask)) {
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                if (strncmp(send_line, "shutdown", 8) == 0) {
                    FD_CLR(0, &allreads);
                    if (shutdown(socket_fd, 1)) {
                        printf("shutdown failed\n");
                        return 0;
                    }
                } else if (strncmp(send_line, "close", 5) == 0) {
                    FD_CLR(0, &allreads);
                    if (close(socket_fd)) {
                        printf("close failed\n");
                        return 0;
                    }
                    sleep(6);
                    exit(0);
                } else {
                    int i = strlen(send_line);
                    if (send_line[i - 1] == '\n') {
                        send_line[i - 1] = 0;
                    }

                    printf("now sending %s\n", send_line);
                    size_t rt = write(socket_fd, send_line, strlen(send_line));
                    if (rt < 0) {
                        printf("write failed\n");
                        return 0;
                    }
                    printf("send bytes: %zu \n", rt);
                }

            }
        }

    }
    return 0;
}