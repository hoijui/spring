#!/bin/sh
# Author: Tobi Vollebregt

# Quit on error.
set -e

# Sanity check.
if [ ! -x /usr/bin/git ]; then
	echo "Error: Could not find /usr/bin/git" 1>&2
	exit 1
fi

# Find correct working directory.
# (Compatible with SConstruct, which is in trunk root)

while [ ! -d installer ]; do
        if [ "$PWD" = "/" ]; then
                echo "Error: Could not find installer directory." 1>&2
                echo "Make sure to run this script from a directory below your checkout directory." 1>&2
                exit 1
        fi
        cd ..
done

set +e # turn of quit on error
git describe --candidates 0 --tags &> /dev/null
if [ $? -ne "0" ]; then
	RELEASE_SOURCE=false
	echo "Making test-packages"
else
	RELEASE_SOURCE=true
	echo "Making release-packages"
fi
set -e # turn it on again

if [ $RELEASE_SOURCE ]; then
	version_string=$(git describe --tags)
	branch=${version_string}
else
	version_string=$(git describe --tags | sed s/\-[^\-]*$//)
	branch="master"
fi
echo "Using $branch as source"

# Each one of these that is set is build when running this script.
# .tar.bz2 and .tar.gz are built with linux (LF) line endings.
# .zip and .7z are built with windows (CRLF) line endings.
dir="spring_${version_string}"
lzma="spring_${version_string}_src.tar.lzma"
#tbz="spring_${version_string}_src.tar.bz2"
tgz="spring_${branch}_src.tar.gz"
#zip="spring_${version_string}_src.zip"
#seven_zip="spring_${branch}_src.7z"

# This is the list of files/directories that go in the source package.
# (directories are included recursively)
include=" \
 $dir/AI/ \
 $dir/Documentation/ \
 $dir/Doxyfile \
 $dir/game/ \
 $dir/installer/ \
 $dir/LICENSE.html \
 $dir/README.* \
 $dir/rts/ \
 $dir/SConstruct \
 $dir/tools/SelectionEditor/ \
 $dir/CMakeLists.txt \
 $dir/VERSION \
 $dir/tools/unitsync/ \
 $dir/tools/DedicatedServer/"

# On linux, win32 executables are useless.
# TASClient is windows only.
exclude_from_all=""
linux_exclude="${exclude_from_all} $dir/installer/pkzip.exe"
linux_include=""
windows_exclude="${exclude_from_all}"
windows_include=""

# Linux line endings, .tar.{bz2,gz} package.
echo ""
echo "Exporting checkout dir with LF line endings"
git clone -n . lf/${dir} > /dev/null
cd lf/$dir
git checkout -b source_package_lf ${branch} > /dev/null
cd ..
# This is used by the build systems, as they fall back to this file,
# if the source root is not a git repository.
# They need the version to incorporate it into the library names,
# for example: libspring-0.80.5.so
echo -n "${version_string}" > ./${dir}/VERSION
[ -n "$linux_exclude" ] && rm -rf $linux_exclude
[ -n "$lzma" ] && echo "Creating archive: $lzma" && \
	tar --lzma -c -f "../$lzma" $include $linux_include
[ -n "$tbz" ]  && echo "Creating archive: $tbz"  && \
	tar -c -j -f "../$tbz" $include $linux_include
[ -n "$tgz" ]  && echo "Creating archive: $tgz"  && \
	tar -c -z -f "../$tgz" $include $linux_include
cd ..
echo "Cleaning"
rm -rf lf

### TODO: needs fixing
# Windows line endings, .zip/.7z package
echo ""
echo "Exporting checkout dir with CRLF line endings"
git clone -n . crlf/${dir} > /dev/null
cd crlf/$dir
git config core.autocrlf true
git checkout -b source_package_crlf ${branch} > /dev/null
cd ..
# This is used by the build systems, as they fall back to this file,
# if the source root is not a git repository.
# They need the version to incorporate it into the library names,
# for example: spring-0.80.5.dll
echo -n "${version_string}" > ./${dir}/VERSION
[ -n "$windows_exclude" ] && rm -rf $windows_exclude
[ -n "$zip" ]       && [ -x /usr/bin/zip ] && echo "Creating archive: $zip"       && \
	/usr/bin/zip -q -r -u -9 "../$zip" $include $windows_include
[ -n "$seven_zip" ] && [ -x /usr/bin/7z ]  && echo "Creating archive: $seven_zip" && \
	/usr/bin/7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on "../$seven_zip" $include >/dev/null
cd ..
echo "Cleaning"
rm -rf crlf
cd ..
