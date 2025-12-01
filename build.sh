gcc -shared -fPIC -O2 rift_open.c -o librift_open.so   # Linux
clang -shared -fPIC -O2 rift_open.c -o librift_open.dylib  # macOS
gcc -shared -O2 rift_open.c -o rift_open.dll -Wl,--out-implib,librift_open.a  # Windows