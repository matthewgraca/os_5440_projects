#!/bin/bash
g++ validators.cpp controller.cpp -o controller.out
g++ validators.cpp reader.cpp -o reader.out
g++ validators.cpp writer.cpp -o writer.out
