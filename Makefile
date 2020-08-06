SOURCE_DIR = src
OBJECT_DIR = $(SOURCE_DIR)/obj
CXX        = g++
CXXFLAGS   = -g -fsanitize=address -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -std=c++17 -Wall
TARGETS    = main.cpp solver.cpp puzzle.cpp edge.cpp vertex.cpp
SOURCES    = $(addprefix $(SOURCE_DIR)/,$(TARGETS))
OBJECTS    = $(patsubst src/%.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

ALL: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o slitherlink_solver $(OBJECTS)

$(OBJECT_DIR)/vertex.o : $(SOURCE_DIR)/vertex.cpp $(SOURCE_DIR)/vertex.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJECT_DIR)/edge.o   : $(SOURCE_DIR)/edge.cpp $(SOURCE_DIR)/edge.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJECT_DIR)/puzzle.o : $(SOURCE_DIR)/puzzle.cpp $(SOURCE_DIR)/puzzle.h $(SOURCE_DIR)/edge.h $(SOURCE_DIR)/vertex.h $(SOURCE_DIR)/const.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJECT_DIR)/solver.o : $(SOURCE_DIR)/solver.cpp $(SOURCE_DIR)/solver.h $(SOURCE_DIR)/puzzle.h $(SOURCE_DIR)/const.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<
$(OBJECT_DIR)/main.o   : $(SOURCE_DIR)/main.cpp 
	$(CXX) $(CXXFLAGS) -o $@ -c $<



.PHONY: run
run   : slitherlink_solver
	./slitherlink_solver samples/easy01.txt

.PHONY: clean
clean : 
	-rm ./src/obj/*.o ./slitherlink_solver

