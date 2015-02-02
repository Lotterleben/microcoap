all:
	clang -Wall -o coap endpoints.c main-posix.c coap.c -DDEBUG -g

clean:
	rm -f coap
