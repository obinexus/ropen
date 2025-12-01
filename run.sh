# shared library
gcc -shared -fPIC -O2 rift_open.c -o librift_open.so

# CLI tool
gcc -DRIFT_OPEN_MAIN -O2 rift_open.c -o riftopen

# test
./riftopen image.png A        # polarity A → normal hex
./riftopen image.png B        # polarity B → conjugate hex