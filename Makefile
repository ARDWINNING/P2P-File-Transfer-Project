CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -pthread
LDFLAGS  := -pthread

TARGET := peerProcess
SRCS   := $(wildcard *.cpp)
OBJS   := $(SRCS:.cpp=.o)

.PHONY: all clean distclean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@

# -MMD -MP: track header dependencies so editing a .h rebuilds what uses it
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(OBJS:.o=.d)

# Remove object files only (keeps the executable)
clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

# Remove object files and the executable
distclean: clean
	rm -f $(TARGET)
