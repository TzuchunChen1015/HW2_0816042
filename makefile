all: server
	rm *.o
server: server.o userstatus.o room.o
	g++ server.o userstatus.o room.o -o server

server.o: src/server.cpp
	g++ -c src/server.cpp

userstatus.o: src/userstatus.cpp
	g++ -c src/userstatus.cpp

room.o: src/room.cpp
	g++ -c src/room.cpp

clean:
	rm server
