CC := gcc
SRC := src
OBJ := obj
DEPS := $(SRC)/socket.h

SRCS=$(wildcard $(SRC)/*.c)
BINS=$(SRCS:$(SRC)/%.c=%)

all: $(BINS)

%: $(SRC)/%.c $(DEPS)
	$(CC) -o $(OBJ)/$@.o $<

clean:
	rm $(OBJ)/*