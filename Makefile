TARGET := SimplePhysics

# ****************************** COMPILE FLAGS ******************************* #

CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -MMD -MP $(SANITIZE_FLAGS) -I./include
CPPFLAGS := -I./include
LDFLAGS :=
# SANITIZE_FLAGS  = -fsanitize=address

# ******************************** FILE LIST ********************************* #

SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

# ********************************* MAKE RULES ******************************* #

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(CPPFLAGS)

-include $(DEPS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

fclean : clean
	rm -f $(NAME)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
