build:
	gcc -Wall -std=c99 src/*.c -o output -lSDL2

run:
	./output

clean: 
	rm output