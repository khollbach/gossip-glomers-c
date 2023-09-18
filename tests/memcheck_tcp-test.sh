#!/usr/bin/env bash

BUILD_DIR="/app/build"

# This is a thin wrapper around tcp-test binary so we can use valgrind with maelstrom
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes $BUILD_DIR/tcp-test.out
