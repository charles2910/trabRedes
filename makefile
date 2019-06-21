program: client.c server.c
	gcc -Wall server.c -o servidor
	gcc -Wall client.c -o cliente
