
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
#define TEMP_MAX 38
#define TEMP_MIN 35
#define TEMP_MED (TEMP_MAX + TEMP_MIN) / 2
#define BAT_MIN 60
#define UMID_MAX 85
#define UMID_MIN 60
#define OXI_MIN 85

// Struct para guardar interface
typedef struct {
	char buffer[16];
	int socket;
	short conectado;
} interface;

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
	interface cliente;
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

	// checa se é conexão do cliente de interface
	if (temp[0] == '3') {
		vec->cliente.socket = new_socket;
		vec->cliente.conectado = 1;
		int flags = guard(fcntl(new_socket, F_GETFL), "get socket flags error");
		guard(fcntl(new_socket, F_SETFL, flags | O_NONBLOCK),
				"set non-blocking");
		return 0;
	}
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
    int server_fd, new_socket = 0, flags, valread =0, on = 1;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char temp[24] = {0};
    char *hello = "Hello from server";
	registro vetor;

	memset(&vetor, 0, sizeof(registro));
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
			if(vetor.lista[i].t_ar.socket > 0) {
				valread = read(vetor.lista[i].t_ar.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%f",&vetor.lista[i].t_ar.valor);
					if (vetor.lista[i].t_ar.valor > TEMP_MAX - 0.5) {
						// ligar circulador
						if (vetor.lista[i].circulador.atuador_ligado == 0) {
							vetor.lista[i].circulador.atuador_ligado = 1;
							char *tmp  = "21\n\0";
							send(vetor.lista[i].circulador.socket, tmp, sizeof(tmp), 0);
						}
					}
					else if (vetor.lista[i].t_ar.valor < TEMP_MIN + 0.5) {
						// ligar aquecedor
						if (vetor.lista[i].aquecedor.atuador_ligado == 0) {
							vetor.lista[i].aquecedor.atuador_ligado = 1;
							char *tmp  = "21\n\0";
							send(vetor.lista[i].aquecedor.socket, tmp, sizeof(tmp), 0);
						}
					}
					else if ((vetor.lista[i].t_ar.valor <  TEMP_MED + 0.5) &&
								(vetor.lista[i].t_ar.valor <  TEMP_MED - 0.5)) {
						// desligar atuadores
						if (vetor.lista[i].circulador.atuador_ligado == 1) {
							vetor.lista[i].circulador.atuador_ligado = 0;
							char *tmp  = "20\n\0";
							send(vetor.lista[i].circulador.socket, tmp, sizeof(tmp), 0);
						}
						if (vetor.lista[i].aquecedor.atuador_ligado == 1) {
							vetor.lista[i].aquecedor.atuador_ligado = 0;
							char *tmp  = "20\n\0";
							send(vetor.lista[i].aquecedor.socket, tmp, sizeof(tmp), 0);
						}
					}
				}
				memset(temp, 0, 16);
				valread = 0;
			}
			if(vetor.lista[i].umidade.socket > 0) {
				valread = read(vetor.lista[i].umidade.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%f",&vetor.lista[i].umidade.valor);
					if (vetor.lista[i].umidade.valor > UMID_MAX - 1) {
						if (vetor.lista[i].umidificador.atuador_ligado == 1) {
							// Desligar Umidificador
							vetor.lista[i].umidificador.atuador_ligado = 0;
							char *tmp  = "20\n\0";
							send(vetor.lista[i].umidificador.socket, tmp, sizeof(tmp), 0);
						}
					}
					else if (vetor.lista[i].umidade.valor < UMID_MIN + 1) {
						// Ligar Umidificador
						if (vetor.lista[i].umidificador.atuador_ligado == 0) {
							vetor.lista[i].umidificador.atuador_ligado = 1;
							char *tmp  = "21\n\0";
							send(vetor.lista[i].umidificador.socket, tmp, sizeof(tmp), 0);
						}
					}
				}
				memset(temp, 0, 16);
				valread = 0;
			}
			if(vetor.lista[i].oxigenacao.socket > 0) {
				valread = read(vetor.lista[i].oxigenacao.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%f",&vetor.lista[i].oxigenacao.valor);
					if (vetor.lista[i].oxigenacao.valor < OXI_MIN) {
						// Soar alarme
						if (vetor.cliente.conectado == 1) {
							char tmp[21] = {0};
							tmp[0] = '6';
							strncpy(&tmp[1], &vetor.lista[i].id, 3);
							tmp[4] = '\0';
							send(vetor.cliente.socket, tmp, sizeof(tmp), 0);
						}
					}
				}
				memset(temp, 0, 16);
				valread = 0;
			}
			if(vetor.lista[i].batimentos.socket > 0) {
				valread = read(vetor.lista[i].batimentos.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%f",&vetor.lista[i].batimentos.valor);
					if (vetor.lista[i].batimentos.valor < BAT_MIN) {
						// Soar alarme
						if (vetor.cliente.conectado == 1) {
							char tmp[21] = {0};
							tmp[0] = '5';
							strncpy(&tmp[1], vetor.lista[i].id, 3);
							tmp[4] = '\0';
							send(vetor.cliente.socket, tmp, sizeof(tmp), 0);
						}
					}
				}
				memset(temp, 0, 16);
				valread = 0;
			}
			if(vetor.lista[i].aquecedor.socket > 0) {
				valread = read(vetor.lista[i].aquecedor.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%hd",&vetor.lista[i].aquecedor.atuador_ligado);
				}
				memset(temp, 0, 16);
				valread = 0;
			}
			if(vetor.lista[i].umidificador.socket > 0) {
				valread = read(vetor.lista[i].umidificador.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%hd",&vetor.lista[i].umidificador.atuador_ligado);
				}
				memset(temp, 0, 16);
				valread = 0;
			}
			if(vetor.lista[i].circulador.socket > 0) {
				valread = read(vetor.lista[i].circulador.socket, temp, 16);
				if (valread > 0) {
					sscanf (&temp[8],"%hd",&vetor.lista[i].circulador.atuador_ligado);
				}
				memset(temp, 0, 16);
				valread = 0;
			}
		}
		printf("%hd\n", vetor.cliente.conectado);
		printf("%s\n", vetor.lista[0].id);
		printf("Hw id: %s; Valor: %f\n", vetor.lista[0].batimentos.sensor_id, vetor.lista[0].batimentos.valor);
		printf("%s\n", vetor.lista[1].id);
		printf("Hw id: %s; Valor: %f\n", vetor.lista[1].t_ar.sensor_id, vetor.lista[1].t_ar.valor);
		//getchar();
	}


    return 0;
}
