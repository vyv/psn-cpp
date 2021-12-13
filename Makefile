SRC_DIR = ./examples/vs2015/psn_client
INC_DIR = ./include

CFLAGS=-c -Wall -I$(INC_DIR) --std=c++11

psn_lib.o:
	gcc $(CFLAGS) -c $(INC_DIR)/psn_lib.hpp

psn_client.o:
	gcc $(CFLAGS) -c $(SRC_DIR)/psn_client.cpp

psn_client: psn_lib.o psn_client.o
	gcc $(CFLAGS) -o psn_client psn_lib.o psn_client.o

