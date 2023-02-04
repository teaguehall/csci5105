all: clean rpcgen server.out client.out

.PHONY: clean
clean:
	rm -f client.out server.out
	rm -f communicate_clnt.c communicate_svc.c communicate_xdr.c communicate.h

.PHONY: rpcgen
rpcgen:
	rpcgen -N communicate.x

server.out: communicate.h communicate_xdr.c communicate_svc.c article.c article.h
	gcc article.c server_routines.c communicate_xdr.c communicate_svc.c -lnsl -o server.out

client.out: communicate.h communicate_xdr.c communicate_clnt.c  article.c article.h
	gcc article.c client.c communicate_xdr.c communicate_clnt.c -o client.out

.PHONY: test
test:
	
