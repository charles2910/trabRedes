
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
#define N_INCUBADORAS 64

// Struct para guardar sensores
typedef struct {
	char sensor_id[4];
	int socket;
	float valor;
} sensor;

// Struct para guardar atuadores
typedef struct {
	char atuador_id[4];
	int socket;
	short atuador_ligado;
} atuador;

// Struct que representa cada incubadora, armazenando sensores e atuadores
typedef struct {
	char id[4];
	sensor t_ar;
	sensor umidade;
	sensor oxigenacao;
	sensor batimentos;
	atuador aquecedor;
	atuador umidificador;
	atuador circulador;
} incubadora;

typedef struct {
	incubadora lista[N_INCUBADORAS];
	int pos_ultima;
} registro;

int guard(int n, char * err) {
	if (n == -1) {
		perror(err);
		exit(1);
	}
	return n;
}

int search(char id[], registro *vec) {
	for(int i = 0; i <= vec->pos_ultima; i++) {
		if (strcmp(id, vec->lista[i].id) == 0)
			return i;
	}
	return -1;
}

int filtrar(int new_socket, registro *vec) {
	char temp[24], inc_id[4], hw_id[4], tipo;
	read(new_socket, temp, 24);
	// checa se é uma requisição de conexão
	if(temp[0] != '1') {
		perror("Erro no protocolo de conexão");
		new_socket = 0;
		return -1;
	}
	int flags = guard(fcntl(new_socket, F_GETFL), "get socket flags error");
	guard(fcntl(new_socket, F_SETFL, flags | O_NONBLOCK),
			"set non-blocking");
	// copia os ids da incubadora e a do hardware (sensor ou atuador)
	strncpy(inc_id, &temp[1], 3);
	inc_id[3] = '\0';
	strncpy(hw_id, &temp[4], 3);
	hw_id[3] = '\0';
	tipo = temp[7];
	// verifica se a incubadora já está registrada
	int pos = search(inc_id, vec);
	if (pos < 0) {
		// precisa registrar nova incubadora
		strcpy(vec->lista[vec->pos_ultima + 1].id, inc_id);
		if(tipo == '1') {
			strcpy(vec->lista[vec->pos_ultima + 1].t_ar.sensor_id, hw_id);
			vec->lista[vec->pos_ultima + 1].t_ar.socket = new_socket;
		}
		else if(tipo == '2') {
			strcpy(vec->lista[vec->pos_ultima + 1].umidade.sensor_id, hw_id);
			vec->lista[vec->pos_ultima + 1].umidade.socket = new_socket;
		}
		else if(tipo == '3') {
			strcpy(vec->lista[vec->pos_ultima + 1].oxigenacao.sensor_id, hw_id);
			vec->lista[vec->pos_ultima + 1].oxigenacao.socket = new_socket;
		}
		else if(tipo == '4') {
			strcpy(vec->lista[vec->pos_ultima + 1].batimentos.sensor_id, hw_id);
			vec->lista[vec->pos_ultima + 1].batimentos.socket = new_socket;
		}
		else if(tipo == '5') {
			strcpy(vec->lista[vec->pos_ultima + 1].aquecedor.atuador_id, hw_id);
			vec->lista[vec->pos_ultima + 1].aquecedor.socket = new_socket;
		}
		else if(tipo == '6') {
			strcpy(vec->lista[vec->pos_ultima + 1].umidificador.atuador_id, hw_id);
			vec->lista[vec->pos_ultima + 1].umidificador.socket = new_socket;
		}
		else if(tipo == '7') {
			strcpy(vec->lista[vec->pos_ultima + 1].circulador.atuador_id, hw_id);
			vec->lista[vec->pos_ultima + 1].circulador.socket = new_socket;
		}
		vec->pos_ultima += 1;
		return 0;
	} else {
		// só adiciona o sensor à incubadora registrada
		if(tipo == '1') {
			if (vec->lista[pos].t_ar.sensor_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].t_ar.sensor_id, hw_id);
			vec->lista[pos].t_ar.socket = new_socket;
		}
		else if(tipo == '2') {
			if (vec->lista[pos].umidade.sensor_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].umidade.sensor_id, hw_id);
			vec->lista[pos].umidade.socket = new_socket;
		}
		else if(tipo == '3') {
			if (vec->lista[pos].oxigenacao.sensor_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].oxigenacao.sensor_id, hw_id);
			vec->lista[pos].oxigenacao.socket = new_socket;
		}
		else if(tipo == '4') {
			if (vec->lista[pos].batimentos.sensor_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].batimentos.sensor_id, hw_id);
			vec->lista[pos].batimentos.socket = new_socket;
		}
		else if(tipo == '5') {
			if (vec->lista[pos].aquecedor.atuador_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].aquecedor.atuador_id, hw_id);
			vec->lista[pos].aquecedor.socket = new_socket;
		}
		else if(tipo == '6') {
			if (vec->lista[pos].aquecedor.atuador_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].umidificador.atuador_id, hw_id);
			vec->lista[pos].umidificador.socket = new_socket;
		}
		else if(tipo == '7') {
			if (vec->lista[pos].aquecedor.atuador_id[0] != NULL) {
				perror("Hardware já cadastrado");
				return -1;
			}
			strcpy(vec->lista[pos].circulador.atuador_id, hw_id);
			vec->lista[pos].circulador.socket = new_socket;
		}
		return 0;
	}
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket = 0, new_socket1 = 0, valread = 1, flags, up = 1;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
	registro vetor;
	vetor.pos_ultima = -1;
	/* for (int i = 0; i < N_INCUBADORAS; i++) {
		printf("%s\n", vetor.lista[i].id);
		printf("%s %d %f\n", vetor.lista[i].t_ar.sensor_id, vetor.lista[i].t_ar.socket, vetor.lista[i].t_ar.valor);
	} */
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

	new_socket = filtrar(new_socket, &vetor);

	while(on != 0) {
		// Checa se há novas conexões
		new_socket = accept(server_fd, (struct sockaddr *)&address,
									(socklen_t*)&addrlen);
		if (new_socket > 0)
			new_socket = filtrar(new_socket, &vetor);

		for (int i = 0; i <= vetor.pos_ultima; i++) {
			
		}
	}
	printf("%s\n", vetor.lista[0].id);

    //while(up > 0) {
		/* flags = guard(fcntl(new_socket, F_GETFL), "get socket flags error");
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
		} */


    //}
    return 0;
}
