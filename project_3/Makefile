.PHONY: all clean

all:
	make clean
	$(MAKE) -C shared
	$(MAKE) -C server
	$(MAKE) -C peer

clean:
	rm -f ./*.out
	$(MAKE) clean -C shared
	$(MAKE) clean -C server
	$(MAKE) clean -C peer
