CC = g++

CXXFLAGS = -std=c++11
LDFLAGS = -lcrypto

TARGET = main
OBJS = main.o utils.o page_table.o page_handling.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm $(TARGET) $(OBJS)