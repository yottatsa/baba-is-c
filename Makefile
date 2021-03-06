CC65_TARGET = c64
CC65_HOME = /usr
PROG = baba
OBJS = $(PROG).o

ifdef CC65_TARGET
CC      = $(CC65_HOME)/bin/cl65
CFLAGS  = -t $(CC65_TARGET) --standard c99 -Oi -Or -Os -Cl
LDFLAGS = -t $(CC65_TARGET) -m $(PROG).map -Ln $(PROG).lbl
else
CC      = g++
CFLAGS  = -Icompat
LDFLAGS = -Wl,-Map,$(PROG).map
endif

.PHONY: default clean

default: $(PROG)

$(PROG): $(OBJS) 

%.o: %.s
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.s: %.c
	$(CC) -c $(CFLAGS) -S -o $@ $<

clean:
	rm -rf $(OBJS) $(PROG)
