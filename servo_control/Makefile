CXX=g++
CXXFLAGS= \
	-O2 \
	$(INCLUDES) \

LDFLAGS= \

INCLUDE_DIRS= \

INCLUDES=$(INCLUDE_DIRS:%=-I %)
LIBS= \

SRC_DIR=./
OUT_DIR=./out
OBJ_DIR=./obj

SRC:=$(wildcard $(SRC_DIR)/*.cpp)
DEPS:=$(wildcard $(INCLUDE)/*.cpp)
OBJ:=$(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# $@ evaluates to the target name (e.g. detection)
# $< evaluates to the first prerequisite (e.g. src files)
# $^ evaluates to all prerequisite (e.g. src files and include files)

detection: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $(OUT_DIR)/motor $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OUT_DIR)/*
