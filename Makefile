CXX=g++
CXXFLAGS= \
	-O2 \
	$(PRE) \

LIBS= \
	-lopencv_core \
	-lopencv_highgui \
	-lopencv_imgcodecs \
	-lopencv_imgproc \
	-lopencv_objdetect \
	-lopencv_videoio \
	-lpthread \

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
	$(CXX) $(CXXFLAGS) $^ -o $(OUT_DIR)/main $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(OUT_DIR)/*
	rm -f ./snapshots/*
