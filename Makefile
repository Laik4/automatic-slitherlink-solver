SOURCE_FOLDER=src
solver: $(SOURCE_FOLDER)/solver.cpp
	g++ -o solver $<
run   : solver
	./solver
clean : solver
	rm ./solver
