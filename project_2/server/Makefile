CC						:= gcc 
CFLAGS					:= -Wall -Werror -g -O
OUT						:= ../server.out
SRCS					:= $(wildcard ./*.c)
OBJS					:= $(patsubst ./%.c,./%.o,$(SRCS))
STATIC_LIBS				+= ../shared/libshared.a
DYNAMIC_LIBS			+= -lpthread

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(STATIC_LIBS) -o $(OUT) $(DYNAMIC_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f ./*.o
