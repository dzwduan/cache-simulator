SRC_DIR = src
BUILD_DIR = build

DIRS = $(shell find $(SRC_DIR) -type d)
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
HEADERS = $(shell find $(SRC_DIR) -name '*.hpp')
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

CXX = g++ 
CXXFLAGS = -std=c++17 -Werror -Wall

all: cache

cache: build_dir $(OBJS)
	@$(CXX) $(OBJS) $(CXXFLAGS) -o $@

build_dir:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@$(CXX) $(CXXFLAGS) $(addprefix -I,${DIRS}) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
