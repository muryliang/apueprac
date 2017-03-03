EXEC = rwseek.exe a.exe
all: $(EXEC)

rwseek.exe: rwseek.c ../mydef.h
	gcc $(CFLAGS) $< -o $@ $(LDFLAGS) $(LIBS)

a.exe: a.c ../mydef.h
	gcc $(CFLAGS) $<  $(LDFLAGS) $(LIBS)  -o $@

clean:
	rm -f $(EXEC) *.o
