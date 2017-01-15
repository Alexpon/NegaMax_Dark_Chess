all: main.cc
	g++ -static -s -std=c++11 -O2 -o search main.cc Protocol.cpp ClientSocket.cpp anqi.cc
	mv search ../EnterRoom/Search
clean:
	rm -r ../EnterRoom/Search/search
