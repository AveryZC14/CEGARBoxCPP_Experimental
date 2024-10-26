CC = g++
CFLAGS = -std=c++17 -Wall -g -O3 -mavx2 -fopenmp -static 
CFLAGS += -I/usr/local/include/antlr4-runtime
LIBS = -L/usr/local/lib -lminisat -lantlr4-runtime -lz
SRCDIR = .
MROOT = $(PWD)

CFLAGS += -I$(MROOT) -I$(MROOT)/GlucoseSolver/glucose -I$(MROOT)/GlucoseSolver/glucose/mtl -I$(MROOT)/GlucoseSolver/glucose/core -I$(MROOT)/GlucoseSolver/glucose/utils -I$(MROOT)/GlucoseSolver/glucose/simp -I$(MROOT)/GlucoseSolver/glucose/parallel

#LIBS += -lantlr4-runtime
# CFLAGS += -I/usr/local/include/antlr4-runtime
#CFLAGS += -I$(ANTLR4_INCLUDE_DIR)

LTL2SNF_TAR = ltl2snf-0.1.0.tar.gz
LTL2SNF_DIR = ltl2snf-0.1.0

# Check for Prover/Ipasir/libipasir.a
HAS_IPASIR := $(wildcard Prover/Ipasir/libipasir.a)

# By default, take all .cpp files excluding Prover/IpasirProver
SOURCES = $(shell find $(SRCDIR) -name "*.cpp" | grep -v Prover/IpasirProver)

# If libipasir.a is found, add sources from Prover/IpasirProver
ifneq ($(HAS_IPASIR),)
    LIBS += -L./Prover/IpasirProver -lipasir
    IPASIR_SRC = $(shell find ./Prover/IpasirProver -name "*.cpp")
    SOURCES += $(IPASIR_SRC)
endif

# add required glucose folders.
SOURCES += $(wildcard $(MROOT)/GlucoseSolver/glucose/simp/*.cc)
SOURCES += $(wildcard $(MROOT)/GlucoseSolver/glucose/core/*.cc)
SOURCES += $(wildcard $(MROOT)/GlucoseSolver/glucose/mtl/*.cc)
SOURCES += $(wildcard $(MROOT)/GlucoseSolver/glucose/utils/*.cc)
SOURCES += $(wildcard $(MROOT)/GlucoseSolver/glucose/parallel/*.cc)
# SOURCES += $(MROOT)/GlucoseSolver/glucose/simp/SimpSolver.cc
SOURCES := $(filter-out $(MROOT)/GlucoseSolver/glucose/simp/Main.cc, $(SOURCES))
SOURCES := $(filter-out $(MROOT)/GlucoseSolver/glucose/parallel/Main.cc, $(SOURCES))

OBJECTS_MAIN = $(filter-out ./ltlmain.o, $(SOURCES:.cpp=.o))
OBJECTS_LTLMAIN = $(filter-out ./main.o, $(SOURCES:.cpp=.o))
EXECUTABLE_MAIN = kaleidoscope
EXECUTABLE_LTLMAIN = lumen

$(info SOURCES = $(SOURCES))

all: $(EXECUTABLE_MAIN) $(EXECUTABLE_LTLMAIN)

$(EXECUTABLE_MAIN): $(OBJECTS_MAIN)
	$(CC) $(CFLAGS) $^ -o $(EXECUTABLE_MAIN) $(LIBS)

$(EXECUTABLE_LTLMAIN): $(OBJECTS_LTLMAIN)
	$(CC) $(CFLAGS) $^ -o $(EXECUTABLE_LTLMAIN) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS_MAIN) $(EXECUTABLE_MAIN) $(OBJECTS_LTLMAIN) $(EXECUTABLE_LTLMAIN)
