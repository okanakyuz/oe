#!/bin/sh

if [ -f "Makefile" ]; then
    make distclean > /dev/null 2>&1
fi

rm -rf autom4te.cache
rm -f Makefile.in
rm -f aclocal.m4
rm -f configure
rm -f config.log
rm -f config.status
rm -f stamp-h1
rm -f depcomp
rm -f install-sh
rm -f missing
rm -f compile
rm -f *.tar.gz

find . -type f -name "*~" -exec rm -f {} +
find . -type f -name "*.o" -exec rm -f {} +

if [ -f "oe" ]; then
    rm -f oe
fi
