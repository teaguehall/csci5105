all: clean server.out client.out

.PHONY: clean
clean:
	rm -f client.out server.out

client.out: client.c client_net.c client_net.h msg.c msg.h
	gcc client.c client_net.c msg.c -lpthread -o client.out

server.out: server.c
	gcc server.c -lpthread -o server.out


	