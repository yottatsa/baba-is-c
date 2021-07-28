#Z88DK_TARGET = zx
#CC65_TARGET = c64
CC65_HOME = /usr
Z88DK_HOME = /usr
PROG = baba
OBJS = $(PROG).o

ifdef Z88DK_TARGET
CC      = $(Z88DK_HOME)/bin/zcc
CFLAGS  = +$(Z88DK_TARGET) 
LDFLAGS = +$(Z88DK_TARGET) -lndos -create-app
else ifdef CC65_TARGET
CC      = $(CC65_HOME)/bin/cl65
CFLAGS  = -t $(CC65_TARGET) --standard c99 -Oi -Or -Os -Cl
LDFLAGS = -t $(CC65_TARGET) -m $(PROG).map -Ln $(PROG).lbl
else
CC      = g++
CFLAGS  = -Icompat -Wno-narrowing
LDFLAGS = -Wl,-Map,$(PROG).map
endif

.PHONY: default clean

default: $(PROG)

$(PROG): $(OBJS) 

%.o: %.s
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJS) $(PROG)
