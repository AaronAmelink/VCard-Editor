CC = gcc
CFLAGS = -Wall -std=c11 -g
LDFLAGS = -L.
INC = include/
SRC = src/
BIN = bin/
LIBS = -L./bin -L/bin/. -lVCParser

#Name: Aaron Amelink
#Student ID: 1266687

parser: libVCParser.so

libVCParser.so: VCParser.o VCHelper.o LinkedListAPI.o VCValidate.o
	$(CC) -shared -o $(BIN)libvcparser.so $(BIN)VCHelper.o $(BIN)VCParser.o $(BIN)VCValidate.o $(BIN)LinkedListAPI.o -L$(BIN)

VCValidate.o: $(SRC)VCValidate.c $(INC)VCParser.h $(INC)VCHelper.h $(INC)LinkedListAPI.h 
	$(CC) -I$(INC) $(CFLAGS) -c -fpic $(SRC)VCValidate.c -o $(BIN)VCValidate.o

VCParser.o: $(SRC)VCParser.c $(INC)VCHelper.h $(INC)VCParser.h $(INC)LinkedListAPI.h 
	$(CC) -I$(INC) $(CFLAGS) -c -fpic $(SRC)VCParser.c -o $(BIN)VCParser.o

VCHelper.o: $(SRC)VCHelper.c $(INC)VCHelper.h $(INC)VCParser.h $(INC)LinkedListAPI.h 
	$(CC) -I$(INC) $(CFLAGS) -c -fpic $(SRC)VCHelper.c -o $(BIN)VCHelper.o

LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	$(CC) -I$(INC) $(CFLAGS) -c -fpic $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

test: test.o libVCParser.so
	$(CC) $(CFLAGS) -L$(BIN) -o $(BIN)test.out $(BIN)test.o $(LIBS)

test.o: $(SRC)testmain.c $(INC)VCParser.h 
	$(CC) $(CFLAGS) -I$(INC) $(LIBS) -c $(SRC)testmain.c -o $(BIN)test.o

clean:
	rm -rf *.o *.so ./$(BIN)*.o ./$(BIN)*.so ./$(BIN)*.out
