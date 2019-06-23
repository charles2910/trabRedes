
// Client side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 50500

int guard(int n, char * err) {
	if (n == -1) {
		perror(err);
		exit(1);
	}
	return n;
}

int main(int argc, char const *argv[])
{
    int sock = 0, valread = 1, up = 1;
    struct sockaddr_in serv_addr;
    char *id = "10015674\n\0";
	char *message = malloc(16 * sizeof(char));
	char *hello1 = "1001567480.1\n\0";
    char buffer[24] = {0};
	memset(message, 0, 16);

    sock = guard(socket(AF_INET, SOCK_STREAM, 0), "\nSocket creation error\n");


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    guard(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr),
						"\nInvalid address/ Address not supported\n");

    guard(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
						"\nConnection Failed \n");

	int flags = guard(fcntl(sock, F_GETFL), "get socket flags error");
	guard(fcntl(sock, F_SETFL, flags | O_NONBLOCK),
			"set non-blocking");

	send(sock , id , strlen(id) , 0 );
	sleep(3);
	send(sock , hello1 , strlen(hello1) , 0 );
    while(up > 0) {
    	gets(message);
	    send(sock , message , strlen(message) , 0 );
	    printf("Hello message sent\n");
	    valread = read( sock , buffer, 16);
	    printf("%s\n",buffer );
		memset(buffer, 0, 16);
    }
    return 0;
}
