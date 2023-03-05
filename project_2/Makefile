.PHONY: all clean

all:
	$(MAKE) -C shared
	$(MAKE) -C server
	$(MAKE) -C client

clean:
	rm -f ./*.out
	$(MAKE) clean -C shared
	$(MAKE) clean -C server
	$(MAKE) clean -C client
