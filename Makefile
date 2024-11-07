all: MCP

MCP: part1.o string_parser.o
	gcc -g -o MCP part1.o string_parser.o

part1.o: part1.c
	gcc -g -c part1.c

# part2.o: part2.c
	# gcc -g -c part2.c

# part3.o: part3.c
	# gcc -g -c part3.c	

# part4.o: part4.c
	# gcc -g -c part4.c

string_parser.o: string_parser.c string_parser.h
	gcc -g -c string_parser.c

clean: 
	rm -f core *.o MCP