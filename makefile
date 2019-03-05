STDARG = -Wall -g -std=c99

#test: test.o deepixel.o
#	gcc -g test.o deepixel.o -o test

#test.o: test.c
#	gcc $(STDARG) -c test.c -o test.o

deepixel/deepixel.out: deepixel/deepixel.c
	gcc $(STDARG) deepixel/deepixel.c -o -g -fsanitize=address deepixel/deepixel.out

deepixel/deepixel.o: deepixel/deepixel.c
	gcc $(STDARG) -c deepixel/deepixel.c -o deepixel/deepixel.o

clean:
	rm deepixel/deepixel.o deepixel/deepixel.out
