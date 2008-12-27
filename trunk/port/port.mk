
#--------------------------------------------------------------------

CC = gcc
AR = ar cru
CFLAGS = -Wall -D_REENTRANT -D_GNU_SOURCE -g -fPIC
LDFLAGS = -lstdc++

LINKER = $(CC)
LINT = lint -c
RM = /bin/rm -f

INSTLIB = mkdir -p ../lib && cp $@ ../lib;

#--------------------------------------------------------------------

# make rule
%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@	

%.o : %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@	

