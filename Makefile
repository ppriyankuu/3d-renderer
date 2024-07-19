build:
	gcc -Wall -std=c99 src/*.c -lSDL2 -o output 

run:
	./output

clean: 
	rm output