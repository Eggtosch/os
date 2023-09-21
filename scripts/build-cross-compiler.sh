set -e

if [ ! -f binutils-2.39.tar.xz ]; then
	wget https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.xz
fi

tar -xf binutils-2.39.tar.xz

export PREFIX=$(realpath ./cross-compiler)
export TARGET=x86_64-elf
export PATH=$PATH:$PREFIX/bin

mkdir -p build-binutils
cd build-binutils

../binutils-2.39/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror
make -j$(nproc)
make -j$(nproc) install

cd ../

if [ ! -f gcc-12.2.0.tar.xz ]; then
	wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz
fi

tar -xf gcc-12.2.0.tar.xz

mkdir -p build-gcc
cd build-gcc

# build without libgcc red zone
# https://wiki.osdev.org/Libgcc_without_red_zone
X86_CONF_FILE=../gcc-12.2.0/gcc/config/i386/t-x86_64-elf
touch $X86_CONF_FILE
echo "MULTILIB_OPTIONS += mno-red-zone" >> $X86_CONF_FILE
echo "MULTILIB_DIRNAMES += no-red-zone" >> $X86_CONF_FILE
ruby ../scripts/add-libgcc-line.rb > ../gcc-12.2.0/gcc/config.gcc.2
rm ../gcc-12.2.0/gcc/config.gcc
mv ../gcc-12.2.0/gcc/config.gcc.2 ../gcc-12.2.0/gcc/config.gcc
../gcc-12.2.0/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c --without-headers
make -j$(nproc) all-gcc
make -j$(nproc) all-target-libgcc
make -j$(nproc) install-gcc
make -j$(nproc) install-target-libgcc

cd ../

rm -f binutils-2.39.tar.xz
rm -f gcc-12.2.0.tar.xz
rm -rf binutils-2.39
rm -rf gcc-12.2.0
rm -rf build-binutils
rm -rf build-gcc

