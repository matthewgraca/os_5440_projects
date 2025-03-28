#!/bin/bash
g++ readwritehelpers.cpp controller.cpp -o controller.out
g++ readwritehelpers.cpp reader.cpp -o reader.out
g++ readwritehelpers.cpp writer.cpp -o writer.out
