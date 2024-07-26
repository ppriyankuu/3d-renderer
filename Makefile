build:
	gcc -Wall -std=c99 src/*.c -lSDL2 -lm -o output 

run:
	./output

clean: 
	rm output