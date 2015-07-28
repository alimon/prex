#!/bin/bash
#
# Build cross compiler script requires tools wget, tar and
# libraries GNU GMP, GNU MPFR, GNU MPC and the ISL,
#
# In Debian based OS'es install with:
# 	# apt-get install wget tar
# 	# apt-get install libgmp-dev libmpfr-dev libmpc-dev libisl-dev
#

BASE_TMP=/tmp/prex-toolchain
MAKE="make -j `nproc`"

usage()
{
	if [ -n "$*" ]; then
		echo "build-toolchain.sh: $*"
	fi

	echo "Usage: build-toolchain.sh [options]"
	echo "Options:"
	echo "	--help			print this message"
	echo "	--target=<i386-elf>	use TARGET for build toolchain"
	echo "	--prefix=<PREFIX>	use PREFIX for install toolchain"
}

setdefaults()
{
	target=""
	prefix=""

	target_binutils_conf=""
	target_gcc_conf=""
}

parseargs()
{
	while [ -n "$1" ]; do
		case $1 in
		--*=*)
			option=`expr "x$1" : 'x\([^=]*\)='`
			optarg=`expr "x$1" : 'x[^=]*=\(.*\)'`
			;;
		--*)
			option=$1
			;;
		*)
			usage "Unrecognized option $1"
			exit 1
			;;
		esac

		case $option in
		--help)
			usage
			exit 0
			;;
		--target)
			target=$optarg
			;;
		--prefix)
			prefix="$optarg"
			;;
		*)
			usage "Unrecognized option $1"
			exit 1
			;;
		esac
		shift
	done
}

load_target_conf()
{
	case "$target" in
	"i386-elf")
		BINUTILS_VERSION=binutils-2.25
		GCC_VERSION=gcc-5.2.0
		GDB_VERSION=
		target_binutils_conf=""
		target_gcc_conf=""
		;;
	"arm-elf")
		BINUTILS_VERSION=binutils-2.22
		GCC_VERSION=gcc-4.7.4
		GDB_VERSION=gdb-7.9

		target_binutils_conf=""
		target_gcc_conf="--with-float=soft --enable-obsolete"
		;;
	*)
		usage "Target $optarg not supported."
		exit 1	
		;;
	esac

}

setdefaults
parseargs "$@"

if [ -z "$target" -o -z "$prefix" ]; then
	usage "Please specify --target and --prefix"
	exit 1
fi

load_target_conf

BINUTILS_SOURCE="ftp://ftp.gnu.org/gnu/binutils/$BINUTILS_VERSION.tar.bz2"
BINUTILS_FILE="$BASE_TMP/$BINUTILS_VERSION.tar.bz2"

GCC_SOURCE="ftp://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.bz2"
GCC_FILE="$BASE_TMP/$GCC_VERSION.tar.bz2"

GDB_SOURCE="ftp://ftp.gnu.org/gnu/gdb/$GDB_VERSION.tar.gz"
GDB_FILE="$BASE_TMP/$GDB_VERSION.tar.gz"

target_tmp="$BASE_TMP/$target"
mkdir -p $target_tmp || exit 1
cd $target_tmp || exit 1

echo "Building $BINUTILS_VERSION ..."
if [ ! -f $BINUTILS_FILE ]; then
	wget -c $BINUTILS_SOURCE -O $BINUTILS_FILE || exit 1
fi
tar -xjf $BINUTILS_FILE -C $target_tmp || exit 1
binutils_build="$target_tmp/binutils_build"
mkdir -p $binutils_build || exit 1
cd $binutils_build || exit 1
../$BINUTILS_VERSION/configure --target="$target" --prefix="$prefix" --with-sysroot --disable-nls \
	--disable-werror $target_binutils_conf || exit 1
$MAKE || exit 1
$MAKE install || exit 1

echo "Building $GCC_VERSION ..."
if [ ! -f $GCC_FILE ]; then
	wget -c $GCC_SOURCE -O $GCC_FILE || exit 1
fi
tar -xjf $GCC_FILE -C $target_tmp || exit 1
gcc_build="$target_tmp/gcc_build"
mkdir -p $gcc_build || exit 1
cd "$gcc_build" || exit 1
../$GCC_VERSION/configure --target="$target" --prefix="$prefix" --enable-languages=c,c++ \
	--disable-nls --without-headers $target_gcc_conf || exit 1
$MAKE all-gcc || exit 1
$MAKE all-target-libgcc || exit 1
$MAKE install-gcc || exit 1
$MAKE install-target-libgcc || exit 1

if [ ! -z "$GDB_VERSION" ]; then
	echo "Building $GDB_VERSION ..."
	if [ ! -f $GDB_FILE ]; then
		wget -c $GDB_SOURCE -O $GDB_FILE || exit 1
	fi

	tar -xzf $GDB_FILE -C $target_tmp || exit 1
	gdb_build="$target_tmp/gdb_build"
	mkdir -p $gdb_build || exit 1
	cd "$gdb_build" || exit 1
	../$GDB_VERSION/configure --target="$target" --prefix="$prefix" || exit 1
	$MAKE || exit 1
	$MAKE install || exit 1
fi
