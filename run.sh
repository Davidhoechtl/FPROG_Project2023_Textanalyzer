#!/bin/bash

echo "Building the C++ project..."
make

if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi

echo "Running the C++ program..."
./out/TextAnalyzer
./out/Test