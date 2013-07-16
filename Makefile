include Makefile.inc
# Output directories                                                                                                                        
BIN_DIR     = ./bin
OBJ_DIR     = ./obj
LIB_DIR     = ./lib

# Dependencies
DEDISP_DIR = /lustre/home/ebarr/Soft/dedisp
TCLAP_DIR  = ./

# Paths
SRC_DIR  = ./src
INCLUDE_DIR = ./include
DATA_TYPES_DIR = ${SRC_DIR}/data_types
TRANSFORMS_DIR = ${SRC_DIR}/transforms

# Compiler flags
OPTIMISE = -O3
DEBUG    = -g

INCLUDE   = -I$(INCLUDE_DIR) -I$(THRUST_DIR) -I$(TCLAP_DIR)/tclap/ -I${DEDISP_DIR}/include
LIBS = -L$(CUDA_DIR)/lib64 -lcuda -lcudart -L${DEDISP_DIR}/lib -ldedisp

CUFLAGS  = --compiler-options -Wall --machine 64 -arch=$(GPU_ARCH) -Xcompiler ${DEBUG}
FLAGS    = -fPIC -Wall




CPPOBJS = ${OBJ_DIR}/filterbank.o ${OBJ_DIR}/timeseries.o ${OBJ_DIR}/dedisperser.o


all: directories ${CPPOBJS} ${LIB_DIR}/libpeasoup.so

directories:
	@mkdir -p ${BIN_DIR}
	@mkdir -p ${OBJ_DIR}
	@mkdir -p ${LIB_DIR}




${OBJ_DIR}/filterbank.o: ${DATA_TYPES_DIR}/filterbank.cpp
	${GXX} -c ${OPTIMISE} ${FLAGS} ${INCLUDE} $< -o $@

${OBJ_DIR}/timeseries.o: ${DATA_TYPES_DIR}/timeseries.cpp
	${GXX} -c ${OPTIMISE} ${FLAGS} ${INCLUDE} $< -o $@

${OBJ_DIR}/dedisperser.o: ${TRANSFORMS_DIR}/dedisperser.cpp
	${GXX} -c ${OPTIMISE} ${FLAGS} ${INCLUDE} $< -o $@ 

${LIB_DIR}/libpeasoup.so: ${CPPOBJS}
	${GXX} -shared -fPIC $^ -o $@ 

clean:
	rm -f ${LIB_DIR}/*.so ${OBJ_DIR}/*.o