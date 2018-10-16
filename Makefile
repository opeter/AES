UNAME_S := $(shell uname -s)
CFLAGS += -Wall -Werror
CFLAGS += -O3
CFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lbsd
endif
LDFLAGS += -lm
CC = clang
SRC = main.c libaes.c
OBJ = $(addprefix .obj/, $(subst .c,.o,$(SRC)))

all: run

.obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

obj: $(OBJ)

run: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o run

clean:
	rm -f $(OBJ) *~ run