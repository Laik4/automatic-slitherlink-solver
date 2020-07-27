SOURCE_FOLDER=src
solver: $(SOURCE_FOLDER)/solver.cpp
	g++ -g -fsanitize=address -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -std=c++17 -Wall -o solver $<
run   : solver
	./solver samples/easy01.txt
clean : solver
	rm ./solver
