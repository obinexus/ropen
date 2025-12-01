# shared library
gcc -shared -fPIC -O2 ropen.c -o librift_open.so

# CLI tool
gcc -DRIFT_OPEN_MAIN -O2 ropen.c -o ropen

# test
./ropen image.png A        # polarity A → normal hex
./ropen image.png B        # polarity B → conjugate hex
