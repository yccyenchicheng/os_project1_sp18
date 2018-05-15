main: main.o fifo.o rr.o sjf.o psjf_2.o util.o
	gcc -o $@ $^

main.o: main.c
	gcc -c main.c

fifo.o: fifo.c
	gcc -g -c fifo.c

rr.o: rr.c
	gcc -c rr.c

sjf.o: sjf.c
	gcc -c sjf.c

psjf_2.o: psjf.c
	gcc -g -c psjf_2.c

util.o: util.c
	gcc -c util.c

clean:
	rm *.o
