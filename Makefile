SOURCE_FOLDER=src
build: $(SOURCE_FOLDER)/solver.cpp
	g++ -g -fsanitize=address -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -std=c++17 -Wall -o solver $(SOURCE_FOLDER)/solver.cpp

.PHONY: run
run   : solver
	./solver samples/easy01.txt

.PHONY: solver
clean : solver
	rm ./solver
