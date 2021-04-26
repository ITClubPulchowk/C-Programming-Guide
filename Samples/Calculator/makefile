INCLUDES = ./includes
SRC = ./src
LIBFLAGS = -lm

calculator : regex.o token.o symbol_table.o parser.o main.o
	cc regex.o token.o symbol_table.o parser.o main.o $(LIBFLAGS) -o calculator

regex.o : $(INCLUDES)/regex.h $(SRC)/regex.c
	cc -I$(INCLUDES)/ -c $(SRC)/regex.c 	

token.o : $(INCLUDES)/regex.h $(INCLUDES)/token.h $(INCLUDES)/io_stream.h $(SRC)/token.c
	cc -I$(INCLUDES)/ -c $(SRC)/token.c

symbol_table.o : $(INCLUDES)/symbol_table.h $(SRC)/symbol_table.c
	cc -I$(INCLUDES)/ -c $(SRC)/symbol_table.c

parser.o : $(INCLUDES)/parser.h $(INCLUDES)/regex.h $(INCLUDES)/io_stream.h $(SRC)/parser.c
	cc -I$(INCLUDES)/ -c $(SRC)/parser.c

main.o : $(INCLUDES)/parser.h $(INCLUDES)/token.h $(INCLUDES)/regex.h $(INCLUDES)/io_stream.h $(SRC)/main.c
	cc -I$(INCLUDES)/ -c $(SRC)/main.c

