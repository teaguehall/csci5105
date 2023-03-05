CC				:=gcc
CFLAGS_OBJS		:= -Wall -Werror -g -O -fPIC
CFLAGS_SHARED	:= -Wall -Werror -shared
OUT 			:= libshared.so
SRCS			:= $(wildcard ./*.c)
OBJS			:= $(wildcard patsub ./%.c,./%.o,$(SRCS))
SHARED_LIBS		+= -lpthread

$(OUT) : $(OBJS)
	$(CC) $(CFLAGS_OBJS) $(OBJS) -o $(OUT) $(SHARED_LIBS)

%.o: %.c
	$(CC) $(CFLAGS_OBJS) -c $< -o $@

.PHONY: clean
clean:
	rm -f ./*.o
	rm -f ./*.so
