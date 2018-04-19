CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lcunit
SRS=test.c ../json.c ../url.c ../terminal.c
OBJ=$(SRS:.c=.o)
EXE=test

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean : 
	$(RM) *.o test
