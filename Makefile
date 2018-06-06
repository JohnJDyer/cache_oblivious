CC = gcc
CFLAGS = -g -std=gnu99
COPTFLAGS = -O3
LDFLAGS = -lm -lstdc++

sort: driver.o sort_utils.o funnel.o timer.c
	$(CC) $(COPTFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cc
	$(CC) $(CFLAGS) $(COPTFLAGS) -o $@ -c $<

clean:
	rm -f *.o *~ sort
