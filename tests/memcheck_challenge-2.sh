#!/usr/bin/env bash

BUILD_DIR="/app/build"

# This is a thin wrapper around challenge-2 binary so we can use valgrind with maelstrom
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes $BUILD_DIR/challenge-2.out
