libmymalloc.a:	my_malloc.o
	ar rcs libmymalloc.a my_malloc.o
my_malloc.o:	my_malloc.c
	gcc -c -o my_malloc.o my_malloc.c

clean:
	rm -rf my_malloc.o libmymalloc.a test_malloc

TEST:	libmymalloc.a test_malloc.c
	gcc -o test_malloc test_malloc.c libmymalloc.a
