File Descriptions:

    main.cpp: 
        - The main program implementing the memory manager. Supports two allocation strategies: first-fit and best-fit.

    a3_gen.sh: 
        - A script to generate random memory allocation and deallocation requests.

    test_input.txt: 
        - A sample input file containing a series of allocation and deallocation commands.

    Makefile: 
        - Contains the build commands for the project. When executed, it will produce two binaries: "firstfit" and "bestfit".

Compilation and Execution:

To compile:
```bash
make

This will produce two binaries: "firstfit" and "bestfit". Each represents a different memory allocation strategy.

To run using the sample input with the "firstfit" strategy:

bash

./firstfit < test_input.txt

For the "bestfit" strategy:

bash

./bestfit < test_input.txt

To generate a new test input using a3_gen.sh:

bash

chmod +x a3_gen.sh
./a3_gen.sh [number_of_commands] > new_input.txt

Replace [number_of_commands] with the number of memory requests you want to generate.
