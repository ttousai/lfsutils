#!/bin/bash

# This script installs all the neccessary files for pkgusr
# except the groupadd and useradd binaries for a full system.
# The groupadd and useradd scripts are installed in the temporary
# LFS environment because shadow is not installed.

if [ z$1 == "zinstall" -o z$1 == "zinstall_lfs" ]; then
	cp bin/* /bin

	if [ z$1 == "zinstall_lfs" ]; then
		cp sbin/* /sbin
	else
		cp sbin/{install_package,add_package_user} /sbin
	fi

	# install global pkgusr configuration files
	mkdir -pv /etc/pkgusr
	cp -a etc/* /etc/pkgusr

	# install pkgusr wrappers, except the C sources.
	mkdir -pv /usr/lib/pkgusr
	ls lib/* | grep -v '*.c' | xargs -I file cp file /usr/lib/pkgusr
	echo "export PATH=/usr/lib/pkgusr:$PATH" >> /.bashrc
fi

if [ z$1 == "zuninstall" ]; then
	#rm all files owned by this script
	for f in `ls bin`; do
		rm /bin/$f
	done

	for f in `ls sbin`; do
		rm /sbin/$f
	done

	rm -rf /etc/pkgusr
	rm -rf /usr/lib/pkgusr

	#delete PATH entry
	sed -i '/.*\/usr\/lib\/pkgusr/d' /.bashrc
fi
