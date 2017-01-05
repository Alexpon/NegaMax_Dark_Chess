all: main.cc
	g++ -static -s -o search main.cc Protocol.cpp ClientSocket.cpp anqi.cc
	mv search ../EnterRoom/Search
clean:
	rm -r ../EnterRoom/Search/search
