#!/bin/bash
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
