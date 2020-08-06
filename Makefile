SOURCE_FOLDER=src
CXX = g++
CXXFLAGS = -g -fsanitize=address -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -std=c++17 -Wall

%.o: $(SOURCE_FOLDER)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

ALL: main.o solver.o puzzle.o edge.o vertex.o 
	$(CXX) $(CXXFLAGS) -o slitherlink_solver $^ && rm $^

.PHONY: run
run   : slitherlink_solver
	./slitherlink_solver samples/easy01.txt

.PHONY: clean
clean : slitherlink_solver
	rm ./slitherlink_solver
