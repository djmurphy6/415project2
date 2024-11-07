all: MCP  # Compiles the default MCP target

# Define separate MCP targets for each part
MCP_part1: part1.o string_parser.o
	gcc -g -o MCP part1.o string_parser.o

MCP_part2: part2.o string_parser.o
	gcc -g -o MCP part2.o string_parser.o

# Default MCP points to the part you’re currently working on (e.g., part2)
MCP: MCP_part2

part1.o: part1.c
	gcc -g -c part1.c

part2.o: part2.c
	gcc -g -c part2.c

string_parser.o: string_parser.c string_parser.h
	gcc -g -c string_parser.c

cpubound: cpubound.c
	gcc -g -c cpubound.c

iobound: iobound.c
	gcc -g -c iobound.c

clean: 
	rm -f core *.o MCP
