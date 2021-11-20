#some name types for readable make file
OB = lib/p1.o lib/p2.o lib/enc1.o lib/enc2.o lib/channel.o lib/aux.o
OBP1 = lib/p1.o lib/aux.o
OBP2 = lib/p2.o lib/aux.o
OBENC1 = lib/enc1.o lib/aux.o
OBENC2 = lib/enc2.o lib/aux.o
OBCHAN = lib/channel.o lib/aux.o
HEADERS = headers/aux.h
EXECP1 = p1
EXECP2 = p2
EXECENC1 = enc1
EXECENC2 = enc2
EXECCHAN = channel
COMP = g++
FLAGS = -std=c++11 -g -Wall -c -ggdb3 -lrt -pthread -lcrypto 
LRT = -pthread -lrt -lcrypto
#executable
all: p1 p2 enc1 enc2 channel

lib/aux.o: source/aux.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/aux.cpp
	mv aux.o lib/aux.o

lib/p1.o: source/p1.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/p1.cpp
	mv p1.o lib/p1.o

lib/p2.o: source/p2.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/p2.cpp
	mv p2.o lib/p2.o

lib/enc1.o: source/enc1.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/enc1.cpp
	mv enc1.o lib/enc1.o 

lib/enc2.o: source/enc2.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/enc2.cpp
	mv enc2.o lib/enc2.o

lib/channel.o: source/channel.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/channel.cpp
	mv channel.o lib/channel.o

p1: $(OBP1)
	$(COMP) -g $(OBP1) -o $(EXECP1) $(LRT)

p2: $(OBP2)
	$(COMP) -g $(OBP2) -o $(EXECP2) $(LRT)

enc1: $(OBENC1)
	$(COMP) -g $(OBENC1) -o $(EXECENC1) $(LRT)

enc2: $(OBENC2)
	$(COMP) -g $(OBENC2) -o $(EXECENC2) $(LRT)

channel: $(OBCHAN)
	$(COMP) -g $(OBCHAN) -o $(EXECCHAN) $(LRT)
#  cleaning command
clean :
	rm -f $(OB) $(EXECCHAN) $(EXECP1) $(EXECP2) $(EXECENC1) $(EXECENC2)