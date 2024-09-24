#! /bin/bash

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose Release/stackvm_c_compiler --dump-ast $1
rm vgcore*
