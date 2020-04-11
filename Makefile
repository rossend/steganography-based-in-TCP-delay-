all: client servidor

client: client.o
	$(CC) client.o -o client

servidor: servidor.o
	$(CC) servidor.o -o servidor

clean:
	rm *.o

