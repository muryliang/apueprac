EXEC = rwseek

all: $(EXEC)

rwseek: rwseek.c
	echo $(CFLAGS) $(LDFLAGS) $(LIBS)
	gcc $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(EXEC) *.o
