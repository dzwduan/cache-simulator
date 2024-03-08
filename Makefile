SRC_DIR = src
# BUILD_DIR = build

# DIRS = $(shell find $(SRC_DIR) -type d)
# SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
# HEADERS = $(shell find $(SRC_DIR) -name '*.hpp')
# OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# CXX = g++ 
# CXXFLAGS = -std=c++17 -Werror -Wall

# all: cache

# cache: build_dir $(OBJS)
# 	@$(CXX) $(OBJS) $(CXXFLAGS) -o $@

# build_dir:
# 	@mkdir -p $(BUILD_DIR)

# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
#@$(CXX) $(CXXFLAGS) $(addprefix -I,${DIRS}) -c $< -o $@

cache: $(SRC_DIR)/cache.cpp $(SRC_DIR)/main.cpp
	@$(CXX) --std=c++20 -g -Werror -Wall -Isrc $^ -o $@

simple: $(SRC_DIR)/simple_cache.cpp
	@$(CXX) --std=c++20 $<  -o $@

.PHONY: simple-run
simple-run: simple
	@echo "====================art_trace======================" > simple_output.txt
	@./simple traces/art.trace >> simple_output.txt
	@echo "====================crafty_trace===================" >> simple_output.txt
	@./simple traces/crafty_mem.trace >> simple_output.txt
	@echo "====================mcf_trace======================" >> simple_output.txt
	@./simple traces/mcf.trace >> simple_output.txt
	@echo "====================swim_trace=====================" >> simple_output.txt
	@./simple traces/swim.trace >> simple_output.txt



.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) simple *.txt cache
