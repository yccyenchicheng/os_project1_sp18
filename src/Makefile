main: main.o fifo.o rr.o sjf_2.o psjf.o util.o
	gcc -o $@ $^

main.o: main.c
	gcc -c main.c

fifo.o: fifo.c
	gcc -c fifo.c

rr.o: rr.c
	gcc -c rr.c

sjf_2.o: sjf_2.c
	gcc -c sjf_2.c

psjf.o: psjf.c
	gcc -c psjf.c

util.o: util.c
	gcc -c util.c

clean:
	rm *.o
