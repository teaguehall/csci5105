all: clean rpcgen server.out client.out

.PHONY: clean
clean:
	rm -f client.out server.out
	rm -f comms_clnt.c comms_svc.c comms_xdr.c comms.h

.PHONY: rpcgen
rpcgen:
	rpcgen -N comms.x

server.out: comms.h comms_xdr.c comms_svc.c
	gcc server_routines.c comms_xdr.c comms_svc.c -lnsl -o server.out

client.out: comms.h comms_xdr.c comms_clnt.c
	gcc client.c comms_xdr.c comms_clnt.c -o client.out

.PHONY: test
test:
	
