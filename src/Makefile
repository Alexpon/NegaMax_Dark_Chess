all: main.cc
	g++ -static -s -std=c++11 -O2 -o search main.cc Protocol.cpp ClientSocket.cpp anqi.cc
	cp search ../EnterRoom/Search
	cp search ../CreateRoom/Search
clean:
	rm -r ../EnterRoom/Search/search
