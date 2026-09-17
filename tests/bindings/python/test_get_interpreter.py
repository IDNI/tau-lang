#!/usr/bin/env python3
"""
Test script checking that the interpreter can be created.
"""

import tau_loader as tau

def main():
	# Make the interpreter for a given specification as a string
	i = tau.get_interpreter("o[t] = i[t].")
	assert i is not None, "Failed to create interpreter"

	print("Test passed!")

if __name__ == "__main__":
	main()
