
#--------------------------------------------------------------------

# WARNING: "ifeq" is a GNU gmake instruction!
# On solaris 2.X, using /usr/ccs/bin/make instead creates an error:
# "Unexpected end of line seen"

ifeq ($(origin version), undefined)
	version = 0.1
endif

#--------------------------------------------------------------------

all:
	@( cd spmime; make )

dist: clean spmime-$(version).src.tar.gz

spmime-$(version).src.tar.gz:
	@find . -type f | grep -v CVS | grep -v .svn | sed s:^./:spmime-$(version)/: > MANIFEST
	@(cd ..; ln -s spmime spmime-$(version))
	(cd ..; tar cvf - `cat spmime/MANIFEST` | gzip > spmime/spmime-$(version).src.tar.gz)
	@(cd ..; rm spmime-$(version))

clean:
	@( cd spmime; make clean )

