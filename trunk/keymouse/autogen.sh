#!/bin/sh
libtoolize --copy --force
echo "Running aclocal ..."
aclocal
echo "Running autoheader ..."
autoheader
echo "Running automake ..."
automake --add-missing --copy
echo "Running autoconf ..."
autoconf
echo "Now run: ./configure --prefix=/usr"
echo "Then: make && make install"
