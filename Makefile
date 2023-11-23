
CXXFLAGS = -Wall -Werror -std=c++20

all: firstfit bestfit

firstfit: main.cpp
	g++ $(CXXFLAGS) -DSTRATEGY=1 -o firstfit main.cpp

bestfit: main.cpp
	g++ $(CXXFLAGS) -DSTRATEGY=2 -o bestfit main.cpp

clean:
	rm -f firstfit bestfit

.PHONY: all clean
