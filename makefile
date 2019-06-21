program: client.c server.c
	gcc -Wall -g server.c -o servidor
	gcc -Wall -g client.c -o cliente
