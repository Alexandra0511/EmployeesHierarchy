build:
	gcc *.c -o tema2

test: build
	valgrind --leak-check=full --track-origins=yes ./tema2

clean:
	rm tema2 output*.out