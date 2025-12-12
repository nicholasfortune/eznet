/*      
        Project:        eznet
        File Purpose:   CLI
        Author:         Nicholas Fortune
        Created:        04-12-2025
        First Release:  04-12-2025
        Updated:        --

        Description:    A simple neural network CLI for easy training on .csv files

        Notes:          --

        -------------------------------------

        © Nicholas Fortune 2025, all rights reserved.
*/

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include "../include/eznet.h"

// Constants
        const char* version = "1.0.0";
        bool bench = false;
        bool shush = false;
        bool force = false;

// Helpers
        void print(const char* str) {
                if (!shush) {
                        std::cout << str;
                }
        }
        void println(const char* str) {
                if (!shush) {
                        std::cout << str << std::endl;
                }
        }
        bool convert_to_uint32_t(const char* str, uint32_t& out) {
                try {
                        unsigned long val = std::stoul(str);
                        if (val > UINT32_MAX) return false;
                        out = static_cast<uint32_t>(val);
                        return true;
                } catch (...) {
                        return false;
                }
        }
        void remove_whitespace(char* str) {
                char* dst = str;
                while (*str) {
                        if (!isspace(static_cast<unsigned char>(*str))) {
                        *dst++ = *str;
                        }
                        str++;
                }
                *dst = '\0';
        }

// Functions
        void help() {
                println("Helper Flags");
                println("    -bench");
                println("        Displays the time taken to complete a command/task");
                println("    -shush");
                println("        Stops any extra prints the command may make");
                println("    -force");
                println("        Forces the command to continue even if there is an error/warning");
                println("");
                println("Helper Commands");
                println("    help");
                println("        Displays list of commands");
                println("    version");
                println("        Displays program version");
                println("");
                println("Main Commands");
                println("    create \"file-name\" <number of neurons per layer>");
                println("        Creates/overwrites an empty neural network file with the given name in the current directory");
                println("        ex: eznet create \"rock-paper-scissors-master.bin\" \"3 4 1\"");
                println("    forward \"file-name\" <inputs>");
                println("        Computes and returns the forward propagation outputs of a given neural network file in the current directory using the given inputs");
                println("        ex eznet forward \"rock-paper-scissors-master.bin\" \"3 1 3\"");
                println("    output \"file-name\"");
                println("        Returns the weights and biases of a given neural network file in the current directory.");
                println("");
        }


int main(int argc, char** argv) {
        if (argc < 2) {std::cout << "error: no command was provided. use \"eznet help\" for help." << std::endl; return 1;}
        std::vector<char*> arguments;
        bench = false;
        shush = false;
        force = false;
        for (int i = 1; i < argc; i++) {
                if (argv[i][0] != '-') {
                        arguments.push_back(argv[i]);
                } else {
                        if (std::string(argv[i]) == "-bench") {
                                bench = true;
                        } else if (std::string(argv[i]) == "-shush") {
                                shush = true;
                        } else if (std::string(argv[i]) == "-force") {
                                force = true;
                        } else {
                                print("Flag \"");
                                print((char*)argv[i]);
                                println("\" is not a valid flag. Run \"eznet help\" for help.");
                        }
                }
        }
        std::string cmd = std::string(arguments[0]);
        auto start = std::chrono::high_resolution_clock::now();



        if (cmd == "help") {
                help();
        } else if (cmd == "version") {
                print("eznet version ");
                println(version);
        } else if (cmd == "create") {
                if (argc <= 3 && !force) {
                        println("error: too few arguments");
                } else {
                        std::vector<uint32_t> layer_sizes;
                        for (int i = 2; i < argc - 1; i++) {
                                uint32_t test;
                                if (convert_to_uint32_t(arguments[i], test)) {
                                        layer_sizes.push_back(test);
                                }
                        }
                        NeuralNetwork::network new_network = NeuralNetwork::create_network(layer_sizes);
                        NeuralNetwork::save_network(arguments[1], new_network);
                }
        } else if (cmd == "forward") {

        } else if (cmd == "output") {

        }



        auto end = std::chrono::high_resolution_clock::now();
        if (bench) {
                std::chrono::duration<double, std::milli> elapsed = end - start;
                std::cout << "-bench: " << elapsed.count() << "ms" << std::endl;
        }
        return 0;
}