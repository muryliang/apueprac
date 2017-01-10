EXEC = rwseek a

all: $(EXEC)

rwseek: rwseek.c
	gcc $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

a: a.c 
	gcc $(CFLAGS) $<  $(LDFLAGS) $(LIBS)  -o $@

clean:
	rm -f $(EXEC) *.o
