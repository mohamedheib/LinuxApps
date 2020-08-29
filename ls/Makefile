ls: main.o ls.o
main.o: main.c ls.h
	cc -c main.c
ls.o: ls.c ls.h
	cc -c ls.c
clean:
	rm -rf *.o ls
