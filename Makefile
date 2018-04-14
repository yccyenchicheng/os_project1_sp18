all:
	gcc schedule.c -o schedule -std=c11

debug:
	gcc -g schedule.c -o schedule -std=c11
