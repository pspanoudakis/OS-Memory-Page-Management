CC = g++

CXXFLAGS = -std=c++11
LDFLAGS = -lcrypto

TARGET = main

MAIN_OBJ = build/main.o
UTILS_OBJ = build/utils.o
PT_OBJ = build/page_table.o
PH_OBJ = build/page_handling.o
OBJS = $(MAIN_OBJ) $(UTILS_OBJ) $(PT_OBJ) $(PH_OBJ)

MAIN_SRC = src/main.cpp
UTILS_SRC = src/utils.cpp
PT_SRC = src/page_table.cpp
PH_SRC = src/page_handling.cpp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) -c -o $(MAIN_OBJ) $(MAIN_SRC)

$(UTILS_OBJ): $(UTILS_SRC)
	$(CC) -c -o $(UTILS_OBJ) $(UTILS_SRC)

$(PT_OBJ): $(PT_SRC)
	$(CC) -c -o $(PT_OBJ) $(PT_SRC)

$(PH_OBJ): $(PH_SRC)
	$(CC) -c -o $(PH_OBJ) $(PH_SRC)

clean:
	rm $(TARGET) $(OBJS)
