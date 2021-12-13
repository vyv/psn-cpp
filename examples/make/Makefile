SRC_DIR = ./
INC_DIR = ../../include

CFLAGS= -Wall -I$(INC_DIR) --std=c++11

psn_lib.o:
	g++ $(CFLAGS) -c $(INC_DIR)/psn_lib.hpp -o psn_lib.o

psn_client.o:
	g++ $(CFLAGS) -c $(SRC_DIR)/psn_client.cpp

psn_client: psn_lib.o psn_client.o
	g++ $(CFLAGS) -o psn_client psn_client.o psn_lib.o 

