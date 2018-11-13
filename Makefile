UNAME_S := $(shell uname -s)
CFLAGS += -Wall -Werror
CFLAGS += -O3
CFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
# ifeq ($(UNAME_S),Linux)
# 	LDFLAGS += -lbsd
# endif
# LDFLAGS += -lm
CC = clang
SRC = libaes.c
OBJ = $(addprefix .obj/, $(subst .c,.o,$(SRC)))

all: run

.obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

obj: $(OBJ)

run: $(OBJ)
	$(CC) $(CFLAGS) -c main.c -o .obj/main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) .obj/main.o -o run

test: $(OBJ)
	$(CC) $(CFLAGS) -c test.c -o .obj/test.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) .obj/test.o -o run_test
	./run_test

clean:
	rm -f $(OBJ) *~ run