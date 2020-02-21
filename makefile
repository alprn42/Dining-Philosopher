all: project3

project3: project3.c
	gcc project3.c -o output -lm -pthread

