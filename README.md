# What is EzNet?
EzNet is a **CPU-based neural network library** written entirely in C++, with the following key features::
- A **public API** for integrating neural networks into your own programs.
- An optional command-line interface *(CLI)* for creating, training, and testing networks **from the terminal**.
- A custom built-in network binary format capable of reading and writing **billions of 32-bit floats** at lightning fast speeds.

You can use EzNet through the library for full programmatic control, or through the CLI for quick experiments or to create prebuilt networks to embed in your projects.


# Building
You can build this project however you want, but for beginners, I recommend g++ from GCC.

### Building the CLI
To get a CLI, open the root directory in terminal, and then choose from the following commands:

For a stable CLI build:
`g++ src/eznet.cpp src/CLI.cpp -o bin/eznet.exe`

For an optimized CLI build:
`g++ src/eznet.cpp src/CLI.cpp -o bin/eznet.exe -O3 -march=native -mtune=native -flto -DNDEBUG`

### Using EzNet as a library
You can compile EzNet into a static library and link it to your own project.

1. Compile EzNet into an object file:
*stable*: `g++ path/to/your/project.cpp path/to/eznet.cpp`
*optimized*: `g++ path/to/your/project.cpp path/to/eznet.cpp -o bin/eznet.exe -O3 -march=native -mtune=native -flto -DNDEBUG`

2. Turn it into a static library:
`ar rcs eznet.a eznet.o`
You will now have the **EzNet** `.a` library file.

4. Link it with your own project:
*stable*: `g++ path/to/your/project.cpp path/to/eznet.a -o bin/eznet.exe`
*optimized*: `g++ path/to/your/project.cpp path/to/eznet.a -o bin/eznet.exe -O3 -march=native -mtune=native -flto -DNDEBUG`


# License
This software is licensed under the [GNU GPL v3](LICENSE) license. **You are allowed to**:
- Use the software **for any purpose**.
- Copy and share the software **freely**.
- **Modify the software** and run your modified version.
- **Distribute modified or unmodified versions**, including commercially.

You **must**:
- **Provide a source** if you distribute binaries.
- Derivative works **must also be GPLv3**.
- You **cannot add extra restrictions**.
- Installation info **must be provided** if distributing in consumer devices.
- **No warranty** is provided.

The GPL only applies when you distribute code.
Please read the [whole license for more details.](LICENSE)
