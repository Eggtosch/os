# include new multilib configuration, needed for building libgcc without redzone
# see the build-cross-compiler.sh script for more info
f = File.open("../gcc-12.2.0/gcc/config.gcc")
lines = f.read.lines.insert(1901, "\ttmake_file=\"${tmake_file} i386/t-x86_64-elf\"")
f.close
puts lines
