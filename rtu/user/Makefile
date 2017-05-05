IDIR=../../include
CC=/usr/local/opt/crosstool/arm-linux/gcc-3.3.4-glibc-2.3.2/bin/arm-linux-gcc
CFLAGS=-I$(IDIR) -I$(IDIR)/realtime/include -I$(IDIR)/linux24/include 
CFLAGS+= -Wall -O3 -g
LDLIBS = -lpthread -L$(IDIR)/realtime/lib
ODIR=obj

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = rtu.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

rtu: $(OBJ)
	$(CC) -o $@.bin $^ $(CFLAGS) $(LDLIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o *~ core $(INCDIR)/*~


