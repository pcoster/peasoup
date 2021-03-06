
include Makefile.inc

# Output directories                                                                                                                        
BIN_DIR     = ./bin
OBJ_DIR     = ./obj

# Paths
SRC_DIR  = ./src
INCLUDE_DIR = ./include

# Compiler flags
OPTIMISE = -O3
DEBUG    = -g 

# Includes and libraries
INCLUDE  = -I$(INCLUDE_DIR) -I$(THRUST_DIR) -I${DEDISP_DIR}/include -I${CUDA_DIR}/include
LIBS = -L$(CUDA_DIR)/lib64 -lcuda -lcudart -L${DEDISP_DIR}/lib -ldedisp -lcufft

# compiler flags
NVCC_COMP_FLAGS = --compiler-options -Wall
NVCCFLAGS  = ${OPTIMISE} --machine 64 -arch=$(GPU_ARCH) -Xcompiler ${DEBUG}
CFLAGS    = -fPIC ${OPTIMISE} ${DEBUG}

OBJECTS   = ${OBJ_DIR}/kernels.o
EXE_FILES = ${BIN_DIR}/dedisp_test

all: directories ${OBJECTS} ${EXE_FILES}

${OBJ_DIR}/kernels.o: ${SRC_DIR}/kernels.cu
	${NVCC} -c ${NVCCFLAGS} ${INCLUDE} $<  -o $@

${BIN_DIR}/dedisp_test: ${SRC_DIR}/dedisp_test.cpp ${OBJECTS}
	${NVCC} ${NVCCFLAGS} ${INCLUDE} ${LIBS} $^ -o $@ 

directories:
	@mkdir -p ${BIN_DIR}
	@mkdir -p ${OBJ_DIR}

clean:
	@rm -rf ${OBJ_DIR}/*.o