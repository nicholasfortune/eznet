# EzNet Building Guide
You can build this project however you want, but for beginners, I recommend g++ from GCC.


# Building the CLI
To get a CLI, open the root directory in terminal, and then choose from the following commands:


### For a stable CLI build:

`g++ src/eznet.cpp src/CLI.cpp -o bin/eznet.exe`


### For an optimized CLI build:

`g++ src/eznet.cpp src/CLI.cpp -o bin/eznet.exe -O3 -march=native -mtune=native -flto -DNDEBUG`


# Using EzNet as a library
You can compile EzNet into a static library and link it to your own project.


### 1. Compile EzNet into an object file:
*stable*: `g++ path/to/eznet.cpp -o bin/eznet.o`

*optimized*: `g++ path/to/eznet.cpp -o bin/eznet.o -O3 -march=native -mtune=native -flto -DNDEBUG`


### 2. Turn it into a static library:
`ar rcs eznet.a eznet.o`

You will now have the **EzNet** `.a` library file.


### 3. Link it with your own project:
*stable*: `g++ path/to/your/project.cpp path/to/eznet.a -o bin/eznet.exe`

*optimized*: `g++ path/to/your/project.cpp path/to/eznet.a -o bin/eznet.exe -O3 -march=native -mtune=native -flto -DNDEBUG`
