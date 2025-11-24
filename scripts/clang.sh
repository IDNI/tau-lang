#!/bin/bash

echo "Using clang compiler"

error() {
	echo -e "\nError: $1\n"
        ./dev help
	exit 1
}

if [ $# -eq 0 ]; then
	error "No script name provided"
fi

script_name="$1"

if [ "$script_name" == "help" ] || [ "$script_name" == "--help" ] || [ "$script_name" == "-h" ]; then
	./dev help
	exit 0
fi

script_name="${script_name%.sh}"  # remove .sh extension if provided
script_path=""

# Check if script exists (script must have .sh extension)
if [ -f "scripts/${script_name}.sh" ]; then
	script_path="scripts/${script_name}.sh"
else
	error "'scripts/${script_name}.sh' not found"
fi

echo "Running: $script_path with clang compiler"

shift  # Remove the first argument (script name)

# Execute the script with clang compiler and pass all remaining arguments
if [ "$script_name" == "docker" ]; then
        exec "$script_path" "clang" "$@"
else
        exec "$script_path" "$@" "-DCMAKE_CXX_COMPILER=$(which clang++) -DCMAKE_C_COMPILER=$(which clang)"
fi