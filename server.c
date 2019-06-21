
// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 50500

// Struct para guardar sensores
typedef struct {
	char sensor_id[4];
	int socket;
	float valor;
} sensor;

// Struct para guardar atuadores
typedef struct {
	char atuador_id[4];
	short atuador_ligado;
} atuador;

// Struct que representa cada incubadora, armazenando sensores e atuadores
struct incubadora {
	char id[4];
	sensor t_ar;
	sensor umidade;
	sensor oxigenacao;
	sensor batimentos;
	atuador aquec_ligado;
	atuador umid_ligado;
	atuador circ_ligado;
};

int guard(int n, char * err) {
	if (n == -1) {
		perror(err);
		exit(1);
	}
	return n;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket = 0, new_socket1 = 0, valread = 1, flags, cut = 1;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    server_fd = guard(socket(AF_INET, SOCK_STREAM, 0), "socket failed");

    // Forcefully attaching socket to the port 50500
    guard(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
			sizeof(opt)), "setsockopt error");

	flags = guard(fcntl(server_fd, F_GETFL), "get socket flags error");
	guard(fcntl(server_fd, F_SETFL, flags | O_NONBLOCK),
			"set non-blocking");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 50500
    guard(bind(server_fd, (struct sockaddr *)&address, sizeof(address)),
			"bind failed");

    guard(listen(server_fd, 3), "listen error");
	while (new_socket < 1) {
    	new_socket = accept(server_fd, (struct sockaddr *)&address,
									(socklen_t*)&addrlen);
	}

    while(valread > 0 || cut > 0) {
		flags = guard(fcntl(new_socket, F_GETFL), "get socket flags error");
		guard(fcntl(new_socket, F_SETFL, flags | O_NONBLOCK),
				"set non-blocking");
	    valread = read( new_socket , buffer, 1024);
		if (valread > 0) {
	    	printf("%s\n",buffer );
	    	send(new_socket , hello , strlen(hello) , 0 );
	    	printf("Hello message sent\n");
		}
		if(new_socket1 < 1)
			new_socket1 = accept(server_fd, (struct sockaddr *)&address,
									(socklen_t*)&addrlen);

		if(new_socket1 > 0) {
			flags = guard(fcntl(new_socket1, F_GETFL), "get socket flags error");
			guard(fcntl(new_socket1, F_SETFL, flags | O_NONBLOCK),
					"set non-blocking");
			valread = read(new_socket1 , buffer, 1024);
			if(valread > 0) {
				printf("%s\n",buffer);
				send(new_socket1 , hello , strlen(hello) , 0);
				printf("Hello message sent\n");
			}
		}
    }
    return 0;
}
