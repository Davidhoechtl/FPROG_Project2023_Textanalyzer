#!/bin/bash

echo "Building the C++ project..."
make build

if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi

echo "Running the C++ tests..."
./out/Tests

echo "Running the C++ program..."
./out/TextAnalyzer