echo "Running aclocal ..."
aclocal
echo "Running autoheader"
autoheader
echo "Running autoconf ..."
autoconf
echo "Now run: ./configure --prefix=/usr"
echo "Then: make && make install"
