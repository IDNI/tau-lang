all:
	#clang++-14 tau.cpp -W -Wall -Wpedantic -std=c++2a -otau-lang -g -ggdb #-O3
	g++-12 tau.cpp dict.cpp -W -Wall -Wpedantic -std=c++2a -otau-lang -g -ggdb -O3
