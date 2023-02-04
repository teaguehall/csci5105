all: clean rpcgen server.out client.out

.PHONY: clean
clean:
	rm -f client.out server.out
	rm -f comms_clnt.c comms_svc.c comms_xdr.c comms.h

.PHONY: rpcgen
rpcgen:
	rpcgen -N comms.x

server.out:
	gcc server.c -o server.out

client.out:
	gcc client.c -o client.out
