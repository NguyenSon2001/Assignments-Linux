.PHONY: all clean objects shared_lib

CUR_DIR:= .
BIN_DIR:= $(CUR_DIR)/bin
INC_DIR:= $(CUR_DIR)/inc
OBJ_DIR:= $(CUR_DIR)/objs
SRC_DIR:= $(CUR_DIR)/src
LIB_DIR:= $(CUR_DIR)/libs

LIB_NAME:= MyShaLib

CC:=gcc
INC_FLASS := -I $(INC_DIR)


all: objects shared_lib install
	$(CC)  $(OBJ_DIR)/main.o -L$(LIB_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/use-shared-library

objects:
	$(CC) -c main.c -o $(OBJ_DIR)/main.o $(INC_FLASS)
	$(CC) -c -fPIC $(SRC_DIR)/sum.c -o $(OBJ_DIR)/sum.o $(INC_FLASS)
	$(CC) -c -fPIC $(SRC_DIR)/hello.c -o $(OBJ_DIR)/hello.o $(INC_FLASS)

shared_lib:
	gcc -shared $(OBJ_DIR)/sum.o $(OBJ_DIR)/hello.o -o $(LIB_DIR)/lib$(LIB_NAME).so
install:
	cp -f $(LIB_DIR)/lib$(LIB_NAME).so /usr/lib

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(LIB_DIR)/*
	rm -rf $(BIN_DIR)/*
	rm -rf /usr/lib/lib$(LIB_NAME).so