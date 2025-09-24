# Makefile for VF2++ Serial and MPI versions with multiple optimization levels
# Authors: Giuseppe
# Description: Compiles VF2++ implementations with different optimization levels and runs Python benchmark scripts

# Compiler settings
CC = gcc
MPICC = mpicc
CFLAGS_BASE = -Wall -Wextra -std=c99
LDFLAGS = -lm

# Graph generation parameters (customizable)
GRAPH_MIN ?= 0
GRAPH_MAX ?= 501
GRAPH_STEP ?= 100
MPI_PROCESSES ?= 2,4

# Optimization levels
OPT_LEVELS = O0 O1 O2 O3

# Source directories
SERIAL_SRC_DIR = VF2pp_Serial/src
MPI_SRC_DIR = VF2pp_MPI/src
SERIAL_INC_DIR = VF2pp_Serial/include
MPI_INC_DIR = VF2pp_MPI/include

# Output directories
SERIAL_OUT_DIR = Compilati_Serial
MPI_OUT_DIR = Compilati_MPI

# Source files
SERIAL_SOURCES = $(wildcard $(SERIAL_SRC_DIR)/*.c)
MPI_SOURCES = $(wildcard $(MPI_SRC_DIR)/*.c)

# Python scripts
PYTHON_SCRIPTS = performance_comparison_VF2pp_Vs_VF2base.py performance_comparison_VF2pp_VF2ppMPI.py

# Default target
all: serial mpi python_scripts

# Create output directories
$(SERIAL_OUT_DIR):
	mkdir -p $(SERIAL_OUT_DIR)

$(MPI_OUT_DIR):
	mkdir -p $(MPI_OUT_DIR)

# Serial compilation targets
serial: $(SERIAL_OUT_DIR) $(addprefix $(SERIAL_OUT_DIR)/VF2pp_, $(OPT_LEVELS))

$(SERIAL_OUT_DIR)/VF2pp_O0: $(SERIAL_SOURCES) | $(SERIAL_OUT_DIR)
	$(CC) $(CFLAGS_BASE) -O0 -I$(SERIAL_INC_DIR) $(SERIAL_SOURCES) -o $@ $(LDFLAGS)

$(SERIAL_OUT_DIR)/VF2pp_O1: $(SERIAL_SOURCES) | $(SERIAL_OUT_DIR)
	$(CC) $(CFLAGS_BASE) -O1 -I$(SERIAL_INC_DIR) $(SERIAL_SOURCES) -o $@ $(LDFLAGS)

$(SERIAL_OUT_DIR)/VF2pp_O2: $(SERIAL_SOURCES) | $(SERIAL_OUT_DIR)
	$(CC) $(CFLAGS_BASE) -O2 -I$(SERIAL_INC_DIR) $(SERIAL_SOURCES) -o $@ $(LDFLAGS)

$(SERIAL_OUT_DIR)/VF2pp_O3: $(SERIAL_SOURCES) | $(SERIAL_OUT_DIR)
	$(CC) $(CFLAGS_BASE) -O3 -I$(SERIAL_INC_DIR) $(SERIAL_SOURCES) -o $@ $(LDFLAGS)

# MPI compilation targets
mpi: $(MPI_OUT_DIR) $(addprefix $(MPI_OUT_DIR)/VF2pp_MPI_, $(OPT_LEVELS))

$(MPI_OUT_DIR)/VF2pp_MPI_O0: $(MPI_SOURCES) | $(MPI_OUT_DIR)
	$(MPICC) $(CFLAGS_BASE) -O0 -I$(MPI_INC_DIR) $(MPI_SOURCES) -o $@ $(LDFLAGS)

$(MPI_OUT_DIR)/VF2pp_MPI_O1: $(MPI_SOURCES) | $(MPI_OUT_DIR)
	$(MPICC) $(CFLAGS_BASE) -O1 -I$(MPI_INC_DIR) $(MPI_SOURCES) -o $@ $(LDFLAGS)

$(MPI_OUT_DIR)/VF2pp_MPI_O2: $(MPI_SOURCES) | $(MPI_OUT_DIR)
	$(MPICC) $(CFLAGS_BASE) -O2 -I$(MPI_INC_DIR) $(MPI_SOURCES) -o $@ $(LDFLAGS)

$(MPI_OUT_DIR)/VF2pp_MPI_O3: $(MPI_SOURCES) | $(MPI_OUT_DIR)
	$(MPICC) $(CFLAGS_BASE) -O3 -I$(MPI_INC_DIR) $(MPI_SOURCES) -o $@ $(LDFLAGS)

# Python scripts execution
python_scripts: serial mpi
	@echo "Running Python benchmark scripts..."
	@echo "Graph parameters: nodes=$(GRAPH_MIN) to $(GRAPH_MAX), step=$(GRAPH_STEP), MPI processes=$(MPI_PROCESSES)"
	@echo "1. Running performance_comparison_VF2pp_Vs_VF2base.py..."
	cd $(shell pwd) && python3 performance_comparison_VF2pp_Vs_VF2base.py $(GRAPH_MIN) $(GRAPH_MAX) $(GRAPH_STEP)
	rm -rf grafi_prove
	@echo "2. Running performance_comparison_VF2pp_VF2ppMPI.py..."
	cd $(shell pwd) && python3 performance_comparison_VF2pp_VF2ppMPI.py $(GRAPH_MIN) $(GRAPH_MAX) $(GRAPH_STEP) $(MPI_PROCESSES)
	rm -rf grafi_prove
	@echo "All Python scripts completed successfully."

# Individual Python script targets
run_pp_base:
	cd $(shell pwd) && python3 performance_comparison_VF2pp_Vs_VF2base.py $(GRAPH_MIN) $(GRAPH_MAX) $(GRAPH_STEP)
	rm -rf grafi_prove


run_pp_mpi:
	cd $(shell pwd) && python3 performance_comparison_VF2pp_VF2ppMPI.py $(GRAPH_MIN) $(GRAPH_MAX) $(GRAPH_STEP) $(MPI_PROCESSES)
	rm -rf grafi_prove

# Clean targets
clean_serial:
	rm -rf $(SERIAL_OUT_DIR)

clean_mpi:
	rm -rf $(MPI_OUT_DIR)

clean: clean_serial clean_mpi
	@echo "Cleaned all compiled binaries."

# Rebuild targets
rebuild_serial: clean_serial serial

rebuild_mpi: clean_mpi mpi

rebuild: clean all

# Test targets (compile only specific optimization levels)
test_O0: $(SERIAL_OUT_DIR)/VF2pp_O0 $(MPI_OUT_DIR)/VF2pp_MPI_O0

test_O1: $(SERIAL_OUT_DIR)/VF2pp_O1 $(MPI_OUT_DIR)/VF2pp_MPI_O1

test_O2: $(SERIAL_OUT_DIR)/VF2pp_O2 $(MPI_OUT_DIR)/VF2pp_MPI_O2

test_O3: $(SERIAL_OUT_DIR)/VF2pp_O3 $(MPI_OUT_DIR)/VF2pp_MPI_O3

# Help target
help:
	@echo "VF2++ Makefile - Available targets:"
	@echo "  all              - Compile all versions and run Python scripts"
	@echo "  serial           - Compile only serial versions (O0-O3)"
	@echo "  mpi              - Compile only MPI versions (O0-O3)"
	@echo "  python_scripts   - Run all Python benchmark scripts"
	@echo "  run_pp_base      - Run performance_comparison_VF2pp_Vs_VF2base.py"
	@echo "  run_mpi_base     - Run performance_comparison_mpi_base.py"
	@echo "  run_pp_mpi       - Run performance_comparison_VF2pp_VF2ppMPI.py"
	@echo "  test_O[0-3]      - Compile specific optimization level"
	@echo "  clean            - Remove all compiled binaries"
	@echo "  clean_serial     - Remove only serial binaries"
	@echo "  clean_mpi        - Remove only MPI binaries"
	@echo "  rebuild          - Clean and rebuild everything"
	@echo "  rebuild_serial   - Clean and rebuild serial versions"
	@echo "  rebuild_mpi      - Clean and rebuild MPI versions"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Customizable parameters:"
	@echo "  GRAPH_MIN=<num>  - Minimum graph size (default: $(GRAPH_MIN))"
	@echo "  GRAPH_MAX=<num>  - Maximum graph size (default: $(GRAPH_MAX))"
	@echo "  GRAPH_STEP=<num> - Step increment (default: $(GRAPH_STEP))"
	@echo "  MPI_PROCESSES=<list> - MPI process counts (default: $(MPI_PROCESSES))"
	@echo ""
	@echo "Example: make all GRAPH_MIN=0 GRAPH_MAX=1001 GRAPH_STEP=250 MPI_PROCESSES=2,4,8"

# Phony targets
.PHONY: all serial mpi python_scripts run_pp_base run_mpi_base run_pp_mpi clean clean_serial clean_mpi rebuild rebuild_serial rebuild_mpi test_O0 test_O1 test_O2 test_O3 help
