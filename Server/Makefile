CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++2a -Werror

SRCS = $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(SRCS:%.cpp=%.d)

TARGET = ipkcpd

.phony: all clean runtcp runudp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp %.d %.hpp
	$(CXX) -MT $@ -MMD -MP -MF $*.d $(CXXFLAGS) -c $(OUTPUT_OPTION) $<
$(DEPS):
include $(wildcard $(DEPS))

runtcp: all
	./$(TARGET) -h 127.0.0.1 -p 2023 -m tcp

runudp: all
	./$(TARGET) -h 127.0.0.1 -p 2023 -m udp

clean:
	rm -f $(OBJS) $(TARGET) $(DEPS)
