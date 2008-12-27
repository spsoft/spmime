
include ../port/port.mk

#--------------------------------------------------------------------

LIBOBJS = spmimebuffer.o spmimeutils.o spmimeheader.o spmimepart.o \
		spmimelite.o spmimecodec.o spmimecharset.o \
		spparseaddr.o spmimeaddr.o spmimetime.o

TARGET = libspmime.a \
		testutils testlite testcodec \
		testaddr testtime

TARGET_ICU = libspmimeicu.a \
		testlite_icu

ifeq ($(shell uname), SunOS)
	CFLAGS += -DLIBICONV_PLUG
endif

#--------------------------------------------------------------------

all: $(TARGET)

icu: $(TARGET_ICU)

libspmime.a: $(LIBOBJS)
	$(AR) $@ $^

libspmimeicu.a: spmimeicu.o
	$(AR) $@ $^

testutils: testutils.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspmime -o $@

testcodec: testcodec.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspmime -o $@

testlite: testlite.o testcomm.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspmime -o $@

testlite_icu: testlite_icu.o testcomm.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspmime -lspmimeicu -licuuc -o $@

testaddr: testaddr.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspmime -o $@

testtime: testtime.o
	$(LINKER) $(LDFLAGS) $^ -L. -lspmime -o $@

clean:
	@( $(RM) *.o vgcore.* core core.* $(TARGET) $(TARGET_ICU) $(SONAME) $(SONAME_S) )

