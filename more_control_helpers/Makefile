SH=/bin/sh

all: compile_lib install_lfs

.PHONY: install install_lfs clean

compile_lib:
	$(MAKE) -C lib

install_lfs: compile_lib
	$(SH) install.sh install_lfs

install: compile_lib
	$(SH) install.sh install

uninstall:
	$(SH) install.sh uninstall

clean:
	rm lib/ldconfig lib/su
