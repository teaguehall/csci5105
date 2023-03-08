CC				:=gcc
CFLAGS			:= -Wall -Werror -g -O
OUT 			:= libshared.a
SRCS			:= $(wildcard ./*.c)
OBJS			:= $(patsubst ./%.c,./%.o,$(SRCS))
SHARED_LIBS		+= -lpthread

$(OUT) : $(OBJS)
	ar -rc $(OUT) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean
clean:
	rm -f ./*.o
	rm -f ./*.a
