fc: f2c.o c2f.o main.o
	gcc f2c.c c2f.c main.o -o fc

f2c.o: f2c.c fc.h
	gcc -ggdb -Wall f2c.c -c

c2f.o: c2f.c fc.h
	gcc -ggdb -Wall c2f.c -c

main.o: main.c fc.h
	gcc -ggdb -Wall main.c -c

clean:
	rm -f *.o fc *~
