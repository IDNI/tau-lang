#!/usr/bin/env python3
"""
Test script checking that the module import initializes the engine without
an error, and that an engine call then works.
"""

import tau_loader as tau

def main():
	init = tau.init_report()
	assert not init.has_error, f"Engine init failed: {init.errors}"

	r = tau.get_interpreter("o[t] = i[t].")
	assert r, f"Failed to create interpreter: {r.report.errors}"
	assert r.value is not None

	print("Test passed!")

if __name__ == "__main__":
	main()
