all: MCP  # Compiles the default MCP target

# Define separate MCP targets for each part
MCP_part1: part1.o string_parser.o
	gcc -g -o MCP part1.o string_parser.o

MCP_part2: part2.o string_parser.o
	gcc -g -o MCP part2.o string_parser.o

MCP_part3: part3.o string_parser.o
	gcc -g -o MCP part3.o string_parser.o

# Default MCP points to the part you’re currently working on (e.g., part2)
MCP: MCP_part3

part1.o: part1.c
	gcc -g -c part1.c

part2.o: part2.c
	gcc -g -c part2.c

part2.o: part3.c
	gcc -g -c part3.c

string_parser.o: string_parser.c string_parser.h
	gcc -g -c string_parser.c

# Create the cpubound executable
cpubound: cpubound.c
	gcc -g -o cpubound cpubound.c

# Create the iobound executable
iobound: iobound.c
	gcc -g -o iobound iobound.c

clean: 
	rm -f core *.o MCP cpubound iobound

