#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

char *g_ip = "127.0.0.1";
uint16_t g_port = 9854;

int main(int argc, char **argv)
{
    if(argc == 3) {
        g_ip = argv[1];
        g_port = atoi(argv[2]);
    }
    else if(argc == 2) {
        g_ip = argv[1];
    }

    printf("server ip:%s, port:%d\n", g_ip, g_port);


    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(g_port)
    };
    
    inet_pton(AF_INET, g_ip, &addr.sin_addr.s_addr);
    

    int ret = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("connect");
        close(fd);
        return -1;
    }

    printf("connect success\n");
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);

    int count =1;
    while( 1) {
        sleep(1);
        char buf[1024];
        memset(buf, 0, sizeof(buf));
        if(count > 10) 
            break;
        sprintf(buf,"hello wuyz ... %d \n",count);
        ret = send(fd, buf, strlen(buf)+1, 0);
        if(ret == -1) {
            perror("send");
            break;
        }
        usleep(1000);
        memset(buf, 0, sizeof(buf));
        int len =0;
        while(1) {
            ret = recv(fd, buf + len, sizeof(buf) -  len, 0);
            if(ret == -1) {
                if(errno == EAGAIN) {
                    printf("server say: %s\n", buf);
                    break;
                }
                perror("recv");
                break;
            }
            else if(ret == 0) {
                break;
            }
            len += ret;
         }
        count++;
    }
    close(fd);
    return 0;
}
