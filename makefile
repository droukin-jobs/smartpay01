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

test:
	./test_scenarios.sh

clean : 
	$(RM) *.o sp_server
