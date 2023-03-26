.PHONY: all clean

all:
	make clean
	$(MAKE) -C shared
	$(MAKE) -C server
	$(MAKE) -C client

clean:
	rm -f ./*.out
	$(MAKE) clean -C shared
	$(MAKE) clean -C server
	$(MAKE) clean -C client
