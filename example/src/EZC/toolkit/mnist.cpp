/** /////////////////////////////////////////////////////////////
//
//  CVision: the flexible cascading-style GUI library for C++
//
// //////////////////////////////////////////////////////////////
//
// Copyright (c) 2017 - 2019 Damian Tran
//
// DESCRIPTION:
//
// CVision is a graphical user interface (GUI) library that
// attempts to simplify and speed up the process of desktop
// app design.  CVision incorporates a cascading structure
// scheme that resembles the following:
//
// App -> View -> Panel -> Element -> Primitives/Sprites
//
// The subsequent connection of each "leaf" of the hierarchy
// automatically ensures that the element will be updated,
// drawn to the renderer, and otherwise disposed of at
// the program's termination.
//
// LEGAL:
//
// Modification and redistribution of CVision is freely 
// permissible under any circumstances.  Attribution to the 
// Author ("Damian Tran") is appreciated but not necessary.
// 
// CVision is an open source library that is provided to you
// (the "User") AS IS, with no implied or explicit
// warranties.  By using CVision, you acknowledge and agree
// to this disclaimer.  Use of CVision in the Users's programs
// or as a part of a derivative library is performed at
// the User's OWN RISK.
//
// ACKNOWLEDGEMENTS:
//
// CVision makes use of SFML (Simple and Fast Multimedia Library)
// Copyright (c) Laurent Gomila
// See licence: www.sfml-dev.org/license.php
//
/////////////////////////////////////////////////////////////  **/

#include "EZC/toolkit/mnist.hpp"

#include <stdio.h>
#include <iostream>
#include <exception>
#include <unistd.h>

int reverse_int(const int& i){
    unsigned char c1 = i & 255,
                c2 = (i >> 8) & 255,
                c3 = (i >> 16) & 255,
                c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

MNIST_database::MNIST_database(){ }

MNIST_database::MNIST_database(const std::string& training_file,
                   const std::string& training_labels,
                   const std::string& test_file,
                   const std::string& test_labels){

    load_training_set(training_file, training_labels);
    load_test_set(test_file, test_labels);
}

bool MNIST_database::load_training_set(const std::string& training_file,
                   const std::string& training_labels){

    if(access(training_file.c_str(), F_OK)){
        std::cout << "Failed to load training file\n";
    }
    if(access(training_labels.c_str(), F_OK)){
        std::cout << "Failed to load training file\n";
    }

    FILE * train_set_file   =   fopen(training_file.c_str(), "rb"),
       * train_label_file   =   fopen(training_labels.c_str(), "rb");

    int magic,
        setSIZE,
        labelSIZE,
        rows,
        columns,
        imgSIZE;

    // Read the training set

    std::cout << "Reading from training set...\n";

    fread(&magic, sizeof(magic), 1, train_set_file);
    magic = reverse_int(magic);

    if(magic != 2051){
        std::cout << "Invalid MNIST image file\n";
        return false;
    }

    std::cout << "Valid MNIST image file format\n";

    fread(&setSIZE, sizeof(setSIZE), 1, train_set_file);    setSIZE = reverse_int(setSIZE);
    fread(&rows, sizeof(rows), 1, train_set_file);          rows = reverse_int(rows);
    fread(&columns, sizeof(columns), 1, train_set_file);    columns = reverse_int(columns);

    imgSIZE = rows*columns;

    std::cout << "Detected set size of " << setSIZE << " [" << rows << " x " << columns << "] images\n";

    training_set.resize(setSIZE);
    for(auto& entry : training_set){

        entry.data = new unsigned char[imgSIZE];
        fread(entry.data, sizeof(unsigned char), imgSIZE, train_set_file);
        entry.x = columns;
        entry.y = rows;

    }

    fclose(train_set_file);

    // Read the training labels

    std::cout << "Reading training label set...\n";

    fread(&magic, sizeof(magic), 1, train_label_file);
    magic = reverse_int(magic);

    if(magic != 2049){
        std::cout << "Invalid MNIST label file";
        training_set.clear();
        return false;
    }

    std::cout << "Valid MNIST label file\n";

    fread(&setSIZE, sizeof(setSIZE), 1, train_label_file);
    setSIZE = reverse_int(setSIZE);

    std::cout << "Detected " << setSIZE << " labels\n";

    for(size_t i = 0; (i < setSIZE && i < training_set.size()); ++i){

        unsigned char label_num;
        fread(&label_num, sizeof(unsigned char), 1, train_label_file);

        training_set[i].tag = std::to_string((int)label_num);

    }

    std::cout << "Load complete\n";

    fclose(train_label_file);

    return true;

}

bool MNIST_database::load_test_set(const std::string& test_file,
                   const std::string& test_labels){

    if(access(test_file.c_str(), F_OK)){
        std::cout << "Failed to load test file\n";
    }
    if(access(test_labels.c_str(), F_OK)){
        std::cout << "Failed to load test file\n";
    }

    FILE * test_set_file      =   fopen(test_file.c_str(), "rb"),
       * test_label_file    =   fopen(test_labels.c_str(), "rb");

    int magic,
        setSIZE,
        labelSIZE,
        rows,
        columns,
        imgSIZE;

    // Read the test set

    std::cout << "Reading test set...\n";

    fread(&magic, sizeof(magic), 1, test_set_file);
    magic = reverse_int(magic);

    if(magic != 2051){
        std::cout << "Invalid MNIST image file\n";
        return false;
    }

    std::cout << "Valid MNIST image file format\n";

    fread(&setSIZE, sizeof(setSIZE), 1, test_set_file);     setSIZE = reverse_int(setSIZE);
    fread(&rows, sizeof(rows), 1, test_set_file);           rows = reverse_int(rows);
    fread(&columns, sizeof(columns), 1, test_set_file);     columns = reverse_int(columns);

    imgSIZE = rows*columns;

    std::cout << "Detected set size of " << setSIZE << " [" << rows << " x " << columns << "] images\n";

    test_set.resize(setSIZE);
    for(auto& entry : test_set){

        entry.data = new unsigned char[imgSIZE];
        fread(entry.data, sizeof(unsigned char), imgSIZE, test_set_file);
        entry.x = columns;
        entry.y = rows;

    }

    fclose(test_set_file);

    // Read the test labels

    std::cout << "Reading test label set...\n";

    fread(&magic, sizeof(magic), 1, test_label_file);
    magic = reverse_int(magic);

    if(magic != 2049){
        std::cout << "Invalid MNIST label file";
        test_set.clear();
        return false;
    }

    std::cout << "Valid MNIST label file\n";

    fread(&setSIZE, sizeof(setSIZE), 1, test_label_file);
    setSIZE = reverse_int(setSIZE);

    std::cout << "Detected " << setSIZE << " labels\n";

    for(size_t i = 0; (i < setSIZE && i < test_set.size()); ++i){

        unsigned char label_num;
        fread(&label_num, sizeof(unsigned char), 1, test_label_file);

        test_set[i].tag = std::to_string((int)label_num);

    }

    std::cout << "Load complete\n";

    fclose(test_label_file);

    return true;

}
