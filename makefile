CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lmicrohttpd
SRS=main.c json.c url.c terminal.c
OBJ=$(SRS:.c=.o)
EXE=sp_server

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@
test_list:
	./test_list_terminals.sh

test_all: test_list

