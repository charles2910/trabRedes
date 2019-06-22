
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 50500

int main(int argc, char const *argv[])
{
    int sock = 0, valread = 1;
    struct sockaddr_in serv_addr;
    char *hello = "12345671\n"/*malloc(24 * sizeof(char))*/;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
	send(sock , hello , strlen(hello) , 0 );
    /*while(valread > 0) {
            gets(hello);
	    send(sock , hello , strlen(hello) , 0 );
	    printf("Hello message sent\n");
	    valread = read( sock , buffer, 1024);
	    printf("%s\n",buffer );
    }*/
    return 0;
}
