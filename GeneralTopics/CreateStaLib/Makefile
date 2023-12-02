.PHONY: all clean objects

CUR_DIR:= .
BIN_DIR:= $(CUR_DIR)/bin
INC_DIR:= $(CUR_DIR)/inc
OBJ_DIR:= $(CUR_DIR)/objs
SRC_DIR:= $(CUR_DIR)/src
LIB_DIR:= $(CUR_DIR)/libs

LIB_NAME:= MyLib

CC:=gcc
INC_FLASS := -I $(INC_DIR)


all: objects static_lib
	$(CC) $(OBJ_DIR)/main.o  -L$(LIB_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/statically-linked

objects:
	$(CC) -c main.c -o $(OBJ_DIR)/main.o $(INC_FLASS)
	$(CC) -c $(SRC_DIR)/sum.c -o $(OBJ_DIR)/sum.o $(INC_FLASS)
	$(CC) -c $(SRC_DIR)/hello.c -o $(OBJ_DIR)/hello.o $(INC_FLASS)

static_lib:
	ar rcs $(LIB_DIR)/lib$(LIB_NAME).a  $(OBJ_DIR)/sum.o $(OBJ_DIR)/hello.o  

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(LIB_DIR)/*
	rm -rf $(BIN_DIR)/*
