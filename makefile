CC = g++
CFLAGS = -Wall -Wextra -g -O2 -fno-strict-aliasing
LFLAGS = -lSDL -lGL -lGLU -lglut

MKDIR = mkdir
TARGET = RunMe

SRC = ./src
OBJ = ./obj
BIN = ./bin

BASE_SRC_DIR = $(SRC)
BASE_OBJ_DIR = $(OBJ)
COMMON_SRC_DIR = $(BASE_SRC_DIR)/Common
COMMON_OBJ_DIR = $(BASE_OBJ_DIR)/Common
INPUT_SRC_DIR = $(BASE_SRC_DIR)/Input
INPUT_OBJ_DIR = $(BASE_OBJ_DIR)/Input
MATH_SRC_DIR = $(BASE_SRC_DIR)/Math
MATH_OBJ_DIR = $(BASE_OBJ_DIR)/Math
RENDERER_SRC_DIR = $(BASE_SRC_DIR)/Renderer
RENDERER_OBJ_DIR = $(BASE_OBJ_DIR)/Renderer
SIM_SRC_DIR = $(BASE_SRC_DIR)/Sim
SIM_OBJ_DIR = $(BASE_OBJ_DIR)/Sim
PARTICLE_SRC_DIR = $(SIM_SRC_DIR)/ParticleSystem
PARTICLE_OBJ_DIR = $(SIM_OBJ_DIR)/ParticleSystem
PATHFINDER_SRC_DIR = $(SIM_SRC_DIR)/PathFinder
PATHFINDER_OBJ_DIR = $(SIM_OBJ_DIR)/PathFinder
SYSTEM_SRC_DIR = $(BASE_SRC_DIR)/System
SYSTEM_OBJ_DIR = $(BASE_OBJ_DIR)/System

INPUT_OBS = $(INPUT_OBJ_DIR)/InputThread.o
MATH_OBS = $(MATH_OBJ_DIR)/matrix44.o $(MATH_OBJ_DIR)/LSQFitter.o
RENDERER_OBS = $(RENDERER_OBJ_DIR)/RenderThread.o $(RENDERER_OBJ_DIR)/ParticleSystemDrawer.o $(RENDERER_OBJ_DIR)/PathFinderDrawer.o
SIM_OBS = $(SIM_OBJ_DIR)/SimThread.o
PARTICLE_OBS = $(PARTICLE_OBJ_DIR)/Particle.o $(PARTICLE_OBJ_DIR)/ParticleSystem.o
SYSTEM_OBS = $(SYSTEM_OBJ_DIR)/Client.o $(SYSTEM_OBJ_DIR)/Engine.o $(SYSTEM_OBJ_DIR)/GEngine.o $(SYSTEM_OBJ_DIR)/Main.o
PATHFINDER_OBS = $(PATHFINDER_OBJ_DIR)/AAStar.o $(PATHFINDER_OBJ_DIR)/Node.o $(PATHFINDER_OBJ_DIR)/PathFinder.o

OBJECTS = $(MATH_OBS) $(RENDERER_OBS) $(SIM_OBS) $(PARTICLE_OBS) $(PATHFINDER_OBS) $(SYSTEM_OBS)


objects: $(OBJECTS)

$(INPUT_OBJ_DIR)/%.o: $(INPUT_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(MATH_OBJ_DIR)/%.o: $(MATH_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(RENDERER_OBJ_DIR)/%.o: $(RENDERER_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(SIM_OBJ_DIR)/%.o: $(SIM_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(PARTICLE_OBJ_DIR)/%.o: $(PARTICLE_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(PATHFINDER_OBJ_DIR)/%.o: $(PATHFINDER_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<

$(SYSTEM_OBJ_DIR)/%.o: $(SYSTEM_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c   -o $@    $<


dirs:
	if [ ! -d $(COMMON_OBJ_DIR) ]; then $(MKDIR) $(COMMON_OBJ_DIR); fi
	if [ ! -d $(INPUT_OBJ_DIR) ]; then $(MKDIR) $(INPUT_OBJ_DIR); fi
	if [ ! -d $(MATH_OBJ_DIR) ]; then $(MKDIR) $(MATH_OBJ_DIR); fi
	if [ ! -d $(RENDERER_OBJ_DIR) ]; then $(MKDIR) $(RENDERER_OBJ_DIR); fi
	if [ ! -d $(SIM_OBJ_DIR) ]; then $(MKDIR) $(SIM_OBJ_DIR); fi
	if [ ! -d $(PARTICLE_OBJ_DIR) ]; then $(MKDIR) $(PARTICLE_OBJ_DIR); fi
	if [ ! -d $(PATHFINDER_OBJ_DIR) ]; then $(MKDIR) $(PATHFINDER_OBJ_DIR); fi
	if [ ! -d $(SYSTEM_OBJ_DIR) ]; then $(MKDIR) $(SYSTEM_OBJ_DIR); fi

target:
	$(CC) $(LFLAGS) -o $(BIN)/$(TARGET)    $(OBJECTS)

all:
	make dirs
	make objects
	make target

clean:
	rm -f $(BIN)/$(TARGET)
	rm -f -r $(BASE_OBJ_DIR)/*
