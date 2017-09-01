# Iris Makefile.

# Build the project.
all: clean release

clean:
	@ rm -fr build/

clean_sample_cases:
	@ echo "Cleaning sample cases of all generated simulation data."
	@ find sample_cases -iname "run-*" | xargs rm -fr $1

dist: clean_sample_cases
	@ echo "Zipping source files and sample cases up for distribution."
	@ zip -r iris-dist Makefile License.txt ReadMe.md scripts
	@ zip -r iris-dist iris-cpp irlib sample_cases

debug:
	@ echo "Building the project using CMake in Debug mode."
	@ mkdir -p build
	(cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug ../iris-cpp && make)

R:
	@ echo "Not yet implemented!"

help:
	@ echo "Iris Makefile options"
	@ echo
	@ echo "make		Clean and build the entire project for release."
	@ echo "make clean	Remove all binaries and compilation files."
	@ echo "make debug      Build the project with debugging symbols."
	@ echo "make release    Build the project optimized for release."
	@ echo "make tests      Build the project with debugging symbols and run all unit tests."
	@ echo
	@ echo "make help	Print this message."

release:
	@ echo "Building the project using CMake in Release mode."
	@ echo "All executable files are stored in the 'build' directory."
	@ echo "To learn about other options, type 'make help'."
	@ mkdir -p build
	(cd build && cmake .. -DCMAKE_BUILD_TYPE=Release ../iris-cpp && make)

tests: debug
	@ echo "Running tests."
	@ sh scripts/run_tests.sh
