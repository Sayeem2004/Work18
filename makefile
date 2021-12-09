all: main.c
	gcc -x c -o main main.c

run:
	./main

clean:
	rm main
