/*      
        Project:        eznet
        File Purpose:   Neural network library
        Author:         Nicholas Fortune
        Created:        04-12-2025
        First Release:  04-12-2025
        Updated:        --

        Description:    A configurable and scaleable all-purpose neural network library

        Notes:          --

        -------------------------------------

        © Nicholas Fortune 2025, all rights reserved.
*/



/*
        **BINARY ARCHITECTURE**
        note: "block = a chunk of bytes with an allocated size"

        block #     block               type                        purpose
        -------------------------------------------------------------------
        --          version             uint32_t                    version # of the file
        --          blocks              uint32_t                    counts the number of blocks (not including blocks 0-2)
        --          block sizes         multiple uint32_t's         shows the size of all the blocks from block 3 and on in bytes.
        --          config size         uint32_t                    shows the size of all the configuration data.
        |             config data 1
        |             config data 2
        |             config data 3
        |           ...
        0           1 x bias layer      1 float 32 for each bias    holds the biases, there can be as many of these as needed
        1           1 x weight layer    1 float 32 for each weight  holds the weights, there can be as many of these as needed
        2           1 x bias layer      1 float 32 for each bias    holds the biases
        3           1 x weight layer    1 float 32 for each weight  holds the weights
        ...

        
        keep weights on every odd block number, and biases on every even block number

        for more info, refer to BINARY.txt in "/docs"
*/

#include "../include/eznet.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <random>
#include <cmath>

// Binary functions
    void insert_bytes(char* location, std::fstream& file, std::streampos position, size_t old_data_size, const char* data, size_t data_size) {
        if (!file.is_open()) {
            std::cerr << "insert_bytes: file not open\n";
            return;
        }

        // Move to end to get file size
        file.seekg(0, std::ios::end);
        std::streampos sp = file.tellg();
        if (sp == -1) {
            std::cerr << "insert_bytes: tellg failed\n";
            return;
        }

        size_t file_size = static_cast<size_t>(sp);

        if (position < 0 || static_cast<size_t>(position) + old_data_size > file_size) {
            std::cerr << "insert_bytes: position + old_data_size exceeds file size\n";
            return;
        }

        // Calculate tail size
        size_t tail_size = file_size - (static_cast<size_t>(position) + old_data_size);

        // Read tail if there is any
        std::vector<char> tail;
        if (tail_size > 0) {
            tail.resize(tail_size);
            file.seekg(position + static_cast<std::streamoff>(old_data_size), std::ios::beg);
            file.read(tail.data(), static_cast<std::streamsize>(tail_size));
            if (!file) {
                std::cerr << "insert_bytes: read tail failed\n";
                return;
            }
        }

        // Write new data
        file.seekp(position, std::ios::beg);
        if (data_size > 0) {
            file.write(data, static_cast<std::streamsize>(data_size));
            if (!file) {
                std::cerr << "insert_bytes: write data failed\n";
                return;
            }
        }

        // Write tail
        if (tail_size > 0) {
            file.write(tail.data(), static_cast<std::streamsize>(tail_size));
            if (!file) {
                std::cerr << "insert_bytes: write tail failed\n";
                return;
            }
        }

        // Resize file if new file is smaller or larger
        size_t new_file_size = file_size - old_data_size + data_size;
        if (new_file_size != file_size) {
            file.flush();
            std::error_code ec;
            std::filesystem::resize_file(location, new_file_size, ec);
            if (ec) {
                std::cerr << "insert_bytes: resize_file failed: " << ec.message() << '\n';
                return;
            }
        }

        file.clear(); // Reset any flags
        return;
    }
    NeuralNetwork::file_metadata read_metadata(std::fstream& file) {
        if (!file.is_open()) {std::cerr << "read_metadata: failed to open provided file.\n";return {};}

        file.seekg(0, std::ios::beg);

        // Version
        uint32_t version;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (!file) {std::cerr << "read_metadata: error getting version metadata.\n";return NeuralNetwork::file_metadata{};}

        // Blocks
        uint32_t blocks;
        file.read(reinterpret_cast<char*>(&blocks), sizeof(blocks));
        if (!file) {std::cerr << "read_metadata: error getting blocks metadata.\n";return NeuralNetwork::file_metadata{};}

        // Block sizes
        std::vector<uint32_t> block_sizes(blocks);
        file.read(reinterpret_cast<char*>(block_sizes.data()), blocks * sizeof(uint32_t));
        if (!file) {std::cerr << "read_metadata: error getting block sizes metadata.\n";return NeuralNetwork::file_metadata{};}

        // Config size
        uint32_t config_size;
        file.read(reinterpret_cast<char*>(&config_size), sizeof(config_size));
        if (!file) {std::cerr << "read_metadata: error getting config size metadata.\n";return NeuralNetwork::file_metadata{};}

        // Config data
        std::vector<uint32_t> config_data(blocks);
        file.read(reinterpret_cast<char*>(config_data.data()), config_size * sizeof(uint32_t));
        if (!file) {std::cerr << "read_metadata: error getting config data metadata.\n";return NeuralNetwork::file_metadata{};}

        return NeuralNetwork::file_metadata{version, blocks, block_sizes};
    }
    void write_config(char* location, std::fstream& file, uint32_t config_size, std::vector<uint32_t> config_data) {
        if (!file.is_open()) {std::cerr << "write_config: failed to open provided file.\n";return;}

        NeuralNetwork::file_metadata metadata = read_metadata(file);

        insert_bytes(location, file, );

        file.seekg(0, std::ios::beg);
    }
    std::vector<float> read_block(char* location, uint32_t block) {
        // Open file
        std::fstream file(location, std::ios::in | std::ios::binary);
        if (!file.is_open()) {std::cerr << "read_block: failed to open \"" << location << "\".\n";return {};}
            
        NeuralNetwork::file_metadata file_metadata = read_metadata(file);
        uint32_t version = file_metadata.version;
        uint32_t blocks = file_metadata.blocks;
        std::vector<uint32_t> block_sizes = file_metadata.block_sizes;
        uint32_t config_size = file_metadata.config_size;

        // Find the block the user wants
            if (block >= blocks) {std::cerr << "Block # requested is invalid.\n";return {};}

            // Sum the size of all blocks before the one the user wants
            size_t sum = sizeof(uint32_t) * (3 + blocks + config_size); // Size of metadata
            for (size_t i = 0; i < block; i++) { sum += block_sizes[i]; }

            // Read the block
            size_t bytes = static_cast<size_t>(block_sizes[block]);
            if (bytes % sizeof(float) != 0) {std::cerr << "read_block: block size not aligned with type\n";return {};}

            std::vector<float> wanted_block(bytes / sizeof(float));

            file.seekg(sum, std::ios::beg);
            if (!file) {std::cerr << "read_block: seekg failed for block " << block << "\n";return {};}

            file.read(reinterpret_cast<char*>(wanted_block.data()), bytes);
            if (!file) {std::cerr << "read_block: error reading block " << block << "\n";return {};}

        return wanted_block;
    }
    void write_block(char* location, uint32_t block, std::vector<float> values) {
        // Open file
        std::fstream file(location, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {std::cerr << "write_block: failed to open \"" << location << "\".\n";return;}
            
        NeuralNetwork::file_metadata file_metadata = read_metadata(file);
        uint32_t version = file_metadata.version;
        uint32_t blocks = file_metadata.blocks;
        std::vector<uint32_t> block_sizes = file_metadata.block_sizes;
        uint32_t config_size = file_metadata.config_size;

        // Find the block the user wants
            size_t sum = sizeof(uint32_t) * (3 + blocks + config_size); // Size of metadata
            for (size_t i = 0; i < block && i < blocks; i++)
                sum += block_sizes[i];

            uint32_t size = static_cast<uint32_t>(values.size() * sizeof(float));

            // Write a new block
            if (block == blocks) {

                // Write new block at end
                file.seekp(sum, std::ios::beg);
                file.write(reinterpret_cast<const char*>(values.data()), size);

                // Append new block sizes entry
                insert_bytes(location, file, (2 + block) * sizeof(uint32_t), 0, reinterpret_cast<const char*>(&size), sizeof(uint32_t));

                // Increment blocks"
                blocks++;
                file.seekp(4, std::ios::beg);
                file.write(reinterpret_cast<const char*>(&blocks), sizeof(uint32_t));
            }

            // Overwrite existing block
            else if (block < blocks) {

                insert_bytes(location, file, sum, block_sizes[block], reinterpret_cast<const char*>(values.data()), size);

                // Overwrite "block sizes" entry
                file.seekp(8 + block * sizeof(uint32_t), std::ios::beg);
                file.write(reinterpret_cast<const char*>(&size), sizeof(uint32_t));
            }

            // Invalid block index
            else {
                std::cerr << "write_block: invalid block index\n";
            }

        file.flush();
        file.close();
    }
    void new_bin(char* location) {
        std::ofstream create(location, std::ios::binary | std::ios::trunc);
        if (!create.is_open()) {std::cerr << "new_bin: cannot create \"" << location << "\"\n";return;}

        std::fstream file(location, std::ios::in | std::ios::out | std::ios::binary);

        if (!file.is_open()) {std::cerr << "new_bin: failed to reopen \"" << location << "\"\n";return;}

        // vrsion
        uint32_t version = 1;
        file.write(reinterpret_cast<char*>(&version), sizeof(version));
        if (!file) {std::cerr << "new_bin: error writing version\n";return;}

        // Blocks
        uint32_t blocks = 0;
        file.write(reinterpret_cast<char*>(&blocks), sizeof(blocks));
        if (!file) {std::cerr << "new_bin: error writing blocks\n";return;}
    }

// Neural network helper functions
    float initialize_weight(uint32_t fan_in, std::mt19937 &gen) {
        return std::normal_distribution<float>(0.0f, std::sqrt(2.0f / fan_in))(gen);
    }
    float activation_function(float input) {
        return std::max(0.0f, input);
    }
    float activation_function_derivative(float input) {
        if (input > 0) {
            return 1.0f;
        } else {
            return 0.0f;
        }
    }
    float loss_function(float y_hat, float y) {
        return ((y_hat - y) * (y_hat - y)) / 2;
    }
    float gradient(float loss, float activation, float pre_activation) {
        return 0.0f;
    }

// Public neural network functions
    namespace NeuralNetwork {
        network create_network(std::vector<uint32_t> layers) {
            size_t length = layers.size();
            if (length < 2) {std::cerr << "create_network: provided network is too small\n";return NeuralNetwork::network{};}

            std::mt19937 gen(std::random_device{}());
            NeuralNetwork::network new_network;
            new_network.config_data[0] = layers[0]; // Set input size

            std::vector<NeuralNetwork::layer> new_layers;
            
            if (layers[1] == 0) {std::cerr << "create_network: layer 0 has zero neurons\n";return NeuralNetwork::network{};}
            for (size_t i = 1; i < length - 1; i++) {
                size_t i_plus_one = i + 1;
                if (layers[i_plus_one] == 0) {std::cerr << "create_network: layer " << i_plus_one << " has zero neurons\n";return NeuralNetwork::network{};}
                NeuralNetwork::layer hidden_layer;
                hidden_layer.input_size = layers[i];
                hidden_layer.output_size = layers[i_plus_one];
                size_t weights = (size_t)layers[i] * (size_t)layers[i_plus_one];
                hidden_layer.weights.resize(weights);

                for (size_t v = 0; v < weights; v++) {
                    hidden_layer.weights[v] = initialize_weight(layers[i], gen);
                }
                hidden_layer.biases.resize(layers[i_plus_one], 0.01f);
                new_layers.push_back(hidden_layer);
            }

            new_network.layers = new_layers;
            return new_network;

        }
        void output_network(NeuralNetwork::network neural_network) {
            for (int i = 0; i < neural_network.layers.size(); i++) {
                int digits = (i == 0) ? 1 : (int)std::log10(abs(i)) + 1; std::cout << i; for (int i = 0; i < 4 - digits; ++i) std::cout << ' ';

                std::cout << "weights:    ";
                for (size_t v = 0; v < neural_network.layers[i].weights.size(); v++) {
                    std::cout << neural_network.layers[i].weights[v] << "  ";
                }
                std::cout << std::endl;
                std::cout << "    biases:     ";
                for (size_t v = 0; v < neural_network.layers[i].biases.size(); v++) {
                    std::cout << neural_network.layers[i].biases[v] << "  ";
                }
                std::cout << std::endl << std::endl;
            }
        }
        void save_network(char* location, NeuralNetwork::network neural_network) {
            if (neural_network.layers.size() < 2) {std::cerr << "save_network: provided network is too small\n";return;}
            new_bin(location);
            size_t block = 0;
            for (size_t i = 0; i < neural_network.layers.size(); i++) {
                if (neural_network.layers[i].biases.size() == 0) {std::cerr << "save_network: layer " << i << "'s # of weights is 0\n";return;}
                write_block(location, block, neural_network.layers[i].weights);
                block++;
                if (neural_network.layers[i].weights.size() == 0) {std::cerr << "save_network: layer " << i << "'s # of biases is 0\n";return;}
                write_block(location, block, neural_network.layers[i].biases);
                block++;
            }
        }
        network load_network(char* location) {
            std::fstream file(location, std::ios::in | std::ios::binary);
            NeuralNetwork::file_metadata file_metadata = read_metadata(file);
            NeuralNetwork::network new_network;

            size_t pointer = 0;
            // Loop through layers
            for (size_t layer = 0; layer < (file_metadata.blocks/2); layer++) {
                new_network.layers[layer].biases = read_block(location, pointer);
                pointer++;
                new_network.layers[layer].weights = read_block(location, pointer);
                pointer++;
            }
        }
        output forward_pass(NeuralNetwork::network neural_network, std::vector<float> inputs) {
            if (neural_network.layers.empty()) {
                std::cerr << "forward_pass: network has no layers\n";
                return NeuralNetwork::output{};
            }
            if (inputs.size() != neural_network.layers[0].input_size) {
                std::cerr << "forward_pass: inputs do not match that of the provided neural network\n";
                return NeuralNetwork::output{};
            }

            // Initialize the forward pass's outputs
            NeuralNetwork::output fp_output;

            // Starting activations are just the inputs
            fp_output.outputs = inputs;

            // Loop through the layers
            for (size_t i = 0; i < neural_network.layers.size(); i++) {
                NeuralNetwork::layer layer = neural_network.layers[i];

                // Allocate space for output activations of this layer
                std::vector<float> next_activations(layer.output_size, 0.0f);

                // Loop through the neurons
                for (size_t j = 0; j < layer.output_size; j++) {

                    // Start the sum with the bias
                    float sum = layer.biases[j];

                    // Add each input * their respective weight
                    for (size_t k = 0; k < layer.input_size; k++) {
                        sum += fp_output.outputs[k] * layer.weights[j * layer.input_size + k]; // Flat array
                    }
                    
                    // Save pre-activation sum
                    fp_output.pre_activations.push_back(sum);

                    // Activation function
                    next_activations[j] = activation_function(sum);

                    // Save activation
                    fp_output.activations.push_back(next_activations[j]);
                }

                // These outputs become inputs for the next layer
                fp_output.outputs = std::move(next_activations);
            }
            
            return fp_output;
        }
        backprop_averages backpropagate(NeuralNetwork::network neural_network, NeuralNetwork::output forward_activations, NeuralNetwork::y answers) {
            if (neural_network.layers.empty()) {
                std::cerr << "backpropagate: network has no layers\n";
                return NeuralNetwork::backprop_averages{};
            }

            NeuralNetwork::backprop_averages averages;

            // Loop through layers
            for (size_t l = neural_network.layers.size(); l-- > 0; l--) {
                float delta;
                if (l == neural_network.layers.size()) {
                    delta = 0.0f;
                } else {

                }
            }

            return averages;
        }

        /*backprop_averages backpropagate(NeuralNetwork::network neural_network, NeuralNetwork::output forward_activations, NeuralNetwork::y answers) {
            if (neural_network.layers.empty()) {
                std::cerr << "backpropagate: network has no layers\n";
                return NeuralNetwork::backprop_averages{};
            }

            NeuralNetwork::backprop_averages averages;

            // Loop through layers
            for (size_t l = neural_network.layers.size(); l-- > 0; l--) {

                // Loop through neurons
                for (size_t n = 0; n < neural_network.layers[l].biases.size(); n++) {
                    float activation = forward_activations.activations[l * neural_network.layers[l].input_size + n];
                    float loss = loss_function(activation, answers.layers[l][n]);
                    float output_delta = activation - answers.layers[l][n];
                    float output_weight_gradient = output_delta * activation;
                    float output_bias_gradient = output_delta;

                    // Loop through previous weights
                    for (size_t j = 0; j < neural_network.layers[l - 1].weights.size(); j++) {
                        float hidden_activation = forward_activations.activations[l * neural_network.layers[l - 1].input_size + j];
                        float hidden_pre_activation = forward_activations.pre_activations[l * neural_network.layers[l - 1].input_size + j];
                        float hidden_delta = output_delta * output_weight_gradient * activation_function_derivative(hidden_pre_activation);

                        float hidden_weight_gradient = hidden_delta * hidden_activation;
                        float hidden_bias_gradient = hidden_delta;
                        
                        averages.weights[l][n].push_back(hidden_weight_gradient);
                        averages.biases[l][n].push_back(hidden_bias_gradient);
                    }
                }
            }

            return averages;
        }*/
    }
    