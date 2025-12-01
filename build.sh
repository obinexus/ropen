gcc -shared -fPIC -O2 ropen.c -o librift_open.so   # Linux
clang -shared -fPIC -O2 ropen.c -o librift_open.dylib  # macOS
gcc -shared -O2 ropen.c -o ropen.dll -Wl,--out-implib,librift_open.a  # Windows
