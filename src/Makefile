all: main.cc
	g++ -static -s -std=c++11 -O2 -o search main.cc Protocol.cpp ClientSocket.cpp anqi.cc
	cp search ../CDC_package_linux_3.1/CDC_interface/ubuntu_14.04/EnterRoom/Search
	cp search ../CDC_package_linux_3.1/CDC_interface/ubuntu_14.04/CreateRoom/Search
clean:
	rm -r ../CDC_package_linux_3.1/CDC_interface/ubuntu_14.04/EnterRoom/Search/search
