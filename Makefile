# CXXFLAGS = -g -std=c++11 -W -Wall -Wunused -Wuninitialized -Wextra -Wshadow -DLOCAL -D_GLIBCXX_DEBUG -fsanitize=address
CXXFLAGS = -Wall -Wshadow -Wextra -std=c++14 -Ofast

all: main

main: main.o outerplanar.o graph.o
	g++ $(CXXFLAGS) -o $@ $^

main.o: main.cpp outerplanar.o graph.o
	g++ $(CXXFLAGS) -c -o $@ $<

outerplanar.o: outerplanar.cpp outerplanar.h graph.o
	g++ $(CXXFLAGS) -c -o $@ $<

graph.o: graph.cpp graph.h
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf *.o main

.PHONY: clean all
