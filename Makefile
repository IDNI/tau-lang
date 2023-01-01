all:
	#clang++-14 tau.cpp -W -Wall -Wpedantic -std=c++2a -otau-lang -g -ggdb #-O3
	#g++-12 -Ibdd/src tau.cpp dict.cpp bdd/src/bdd.cpp -flto -W -Wall -Wpedantic -std=c++2a -otau-lang -g -ggdb -O3
	#clang++-14 babdd.cpp tau.cpp -flto -W -Wall -Wpedantic -std=c++2a -otau-lang -g -ggdb #-O3
	g++-12 tau.cpp dict.cpp -flto -W -Wall -Wpedantic -std=c++2a -otau-lang -g -ggdb #-O3
