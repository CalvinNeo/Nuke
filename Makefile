CC = gcc
CXX = g++-7

cov_comp = -fprofile-arcs -ftest-coverage -fno-inline 
cov_lnk = -fprofile-arcs -ftest-coverage --coverage -fno-inline 

NO_WARN = -w
TRIM_WARN = -Wno-unused-variable -Wno-unused-but-set-variable -Wno-format-security -Wno-format
CFLAGS = -DPOSIX -g -fpermissive -std=c++1z 

ifeq ($(MODE), COV)
	# "Coverage mode"
	CFLAGS_COV = $(cov_comp)
	CFLAGS_COV_LNK = $(cov_lnk)
	CFLAGS += -O0
	CFLAGS += $(TRIM_WARN)
else ifeq ($(MODE), DEBUG)
	# "Debug mode"
	CFLAGS += -O0
else
	# "Normal mode"
	CFLAGS_COV = 
	CFLAGS_COV_LNK = 
	CFLAGS += -O2
	CFLAGS += $(NO_WARN)
endif

OBJ_EXT=o

ROOT = .
# Important not to include ".", or gcov -r will fail with some files
SRC_ROOT = src
BIN_ROOT = bin
OBJ_ROOT = $(BIN_ROOT)/obj
DYOBJ_ROOT = $(BIN_ROOT)/dyobj
TEST_ROOT = test

threadpool_test: $(BIN_ROOT)
	$(CXX) $(CFLAGS) $(CFLAGS_COV_LNK) -o $(BIN_ROOT)/threadpool_test $(TEST_ROOT)/thread_pool.cpp -L/usr/lib/ -I$(SRC_ROOT) -lpthread

preproc_test: $(BIN_ROOT)
	$(CXX) -E -C $(TEST_ROOT)/preproc.cpp > $(BIN_ROOT)/preproc.expand.cpp

$(OBJ_ROOT)/%.o: $(SRC_ROOT)/%.cpp $(OBJ_ROOT)
	$(CXX) -c $(CFLAGS) $(CFLAGS_COV) $< -o $@

$(BIN_ROOT):
	mkdir -p $(BIN_ROOT)

$(OBJ_ROOT):
	mkdir -p $(OBJ_ROOT)

.PHONY: clean
clean: clean_cov
	rm -rf $(BIN_ROOT)
