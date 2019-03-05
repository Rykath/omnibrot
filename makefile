STDARG = -Wall -g -std=c99

#test: test.o deepixel.o
#	gcc -g test.o deepixel.o -o test

#test.o: test.c
#	gcc $(STDARG) -c test.c -o test.o

deepixel/deepixel.out: deepixel/deepixel.c
	gcc $(STDARG) deepixel/deepixel.c -o -g -fsanitize=address deepixel/deepixel.out

deepixel/deepixel.o: deepixel/deepixel.c
	gcc $(STDARG) -c deepixel/deepixel.c -o deepixel/deepixel.o

deepixel/testing: deepixel/testing/basics.c deepixel/deepixel.o
	gcc $(STDARG) -c deepixel/testing/basics.c -o deepixel/testing/basics.o
	gcc $(STDARG) deepixel/testing/basics.o deepixel/deepixel.o -o deepixel/testing/basics.out

clean:
	rm deepixel/deepixel.o deepixel/deepixel.out
	rm deepixel/testing/basics.o deepixel/testing/basics.out
