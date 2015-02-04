all:
	clang -Wall -o coap endpoints.c microcoap-client.c coap.c -DDEBUG -g

clean:
	rm -f coap
