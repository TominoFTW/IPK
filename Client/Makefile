CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++2a

SRCS = ipkcpc.cpp
OBJS = $(SRCS:.cpp=.o)

TARGET = ipkcpc

.phony: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

runtcp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m tcp

runudp: all
	./$(TARGET) -h 0.0.0.0 -p 2023 -m udp

clean:
	rm -f $(OBJS) $(TARGET)
