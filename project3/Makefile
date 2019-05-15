
all: libralloc.a  app overhead_test

libralloc.a:  ralloc.c
	gcc -Wall -c ralloc.c
	ar -cvq libralloc.a ralloc.o
	ranlib libralloc.a

app: app.c
	gcc -Wall -o app app.c -L. -lralloc -lpthread

overhead_test:overhead_test.c
	gcc -Wall -o overhead_test overhead_test.c -L. -lralloc -lpthread

clean: 
	rm -fr *.o *.a *~ a.out  app ralloc.o ralloc.a libralloc.a
