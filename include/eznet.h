/*      
        Project:        eznet
        File Purpose:   Neural network library header file
        Author:         Nicholas Fortune
        Created:        05-12-2025
        First Release:  04-12-2025
        Updated:        --

        Description:    The header file for a configurable and scaleable all-purpose neural
                        network library

        Notes:          --

        -------------------------------------

        © Nicholas Fortune 2025, all rights reserved.
*/

#pragma once

#include <cstdint>
#include <vector>

namespace NeuralNetwork {
    struct file_metadata {
        uint32_t version;
        uint32_t blocks;
        std::vector<uint32_t> block_sizes;
        uint32_t config_size;
        std::vector<uint32_t> config_data;
    };
    struct layer {
        std::vector<float> weights;
        std::vector<float> biases;
        uint32_t input_size;
        uint32_t output_size;
    };
    struct network {
        std::vector<layer> layers;
        std::vector<uint32_t> config_data;
    };
    struct output {
        std::vector<float> outputs;
        std::vector<float> activations;
        std::vector<float> pre_activations;
    };
    struct backprop_averages {
        std::vector<std::vector<std::vector<float>>> weights;
        std::vector<std::vector<std::vector<float>>> biases;
    };
    struct y {
        std::vector<std::vector<float>> layers;
    };

    //Creates an initialized, untrained neural network with the amount of layers being the amount of items in an array, and each item's value being the amount of neurons in that layer and the first layer being excluded as the input size.
    NeuralNetwork::network create_network(std::vector<uint32_t> layers);

    // Deletes the old neural network .bin file, and saves the given neural network to the .bin file.
    void save_network(char* location, NeuralNetwork::network neural_network);

    // Prints out the contents of the provided neural network.
    void output_network(char* location, NeuralNetwork::network neural_network);

    // Passes inputs through a given neural network and returns the outputs.
    output forward_pass(NeuralNetwork::network neural_network, std::vector<float> inputs);
}