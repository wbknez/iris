#/usr/bin/env sh

if [[ ! -d build/test ]]; then
    echo "Please build the project first!"
    echo "Run \"make debug\" from the project\'s root."
else
    build/test/iris_test
fi
