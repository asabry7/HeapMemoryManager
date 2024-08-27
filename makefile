output: main.c heap.c heap.h
	gcc -g main.c heap.c -o output
	
clean:
	rm output
	rm stress