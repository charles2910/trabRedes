
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
    char *id = "3\n\0";
	char *message = malloc(24 * sizeof(char));
    char buffer[26] = {0};
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

	send(sock , id , strlen(id) , 0);

	/* int fd1 = fileno(stdin);
	int fd2 = fileno(stdout);
	printf("%d %d\n", fd1, fd2); */

	// setando stdin como nao bloqueante
	int flags1;
	flags1 = fcntl(0, F_GETFL, 0);
	flags1 |= O_NONBLOCK;
	fcntl(0, F_SETFL, flags1);

	printf("Conexão com o gerenciador estabelecida\n");
	printf("Digite o id da incubadora cujos parâmetros devem ser consultados.\n");
	printf("(id: 001, 002, 003 ...)\n");

    while(up > 0) {
    	gets(&message[1]);
		if(message[1] != 0) {
			message[0] = '4';
	    	send(sock , message , strlen(message) , 0 );
	    	printf("Get parameters message sent\n");
			memset(message, 0, 24);
		}
	    valread = read( sock , buffer, 26);
		if (valread > 0) {
			if (buffer[0] == '4') {
				char inc[4] = {0}, t_ar[5] ={0}, umid[5] = {0}, oxig[5] = {0}, bat[5] = {0};

				strncpy(inc, &buffer[1], 4);
				strncpy(t_ar, &buffer[5], 5);
				strncpy(umid, &buffer[10], 5);
				strncpy(oxig, &buffer[15], 5);
				strncpy(bat, &buffer[20], 5);

	    		printf("Parâmetros na incubadora %s\n",inc);
				printf("Temperatura: %s\n",t_ar);
				printf("Umidade: %s\n",umid);
				printf("Oxigenação: %s\n",oxig);
				printf("Batimentos: %s\n",bat);
				memset(buffer, 0, 26);
			}
			else if (buffer[0] == '5') {
				char bat_id[4] = {0};
				strcpy(bat_id, &buffer[1]);
				printf("Alerta Urgente\nBatimentos baixos na incubadora %s\n", bat_id);
			}
			else if (buffer[0] == '6') {
				char oxi_id[4] = {0};
				strcpy(oxi_id, buffer[1]);
				printf("Alerta Urgente\nOxigenação baixa na incubadora %s\n", oxi_id);
			}
		}
    }
    return 0;
}
