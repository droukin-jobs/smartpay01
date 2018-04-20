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

test_scenarios:
	test_scenarios.sh

clean : 
	$(RM) *.o test
