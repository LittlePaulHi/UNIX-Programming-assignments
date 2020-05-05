CC	= gcc
CFLAGS	= -Wall -g
LDL	= -ldl

PROGS = sandbox sandbox.so

all: $(PROGS)

%: %.c
	$(CC) -o $@ $(CFLAGS) $< $(LDFLAGS)

%.so: sandboxso.c
	$(CC) -o $@ -shared -fPIC $^ $(LDL)

clean:
	rm -f *~ $(PROGS)