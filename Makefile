
PLATFORM := $(shell uname)

# Use pkg-config to get the right libraries for your platform
GL_LIBS = -lGL `pkg-config --static --libs glfw3` -lGLEW 
EXT = 
DEFS = `pkg-config --cflags glfw3`

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32
endif

binDir = ./bin/

libDir = ./lib/
vieDir = ./view/
modDir = ./model/
shaDir = ./shader/
utiDir = ./utility/
skyboxDir = ./model/SkyBox/
particleDir = ./model/Particles/
objectDir = ./model/Object/
LINK += $(binDir)ParticleGenerator.o $(binDir)SkyBox.o $(binDir)Object.o $(binDir)Camera.o $(binDir)helpers.o $(binDir)tiny_obj_loader.o $(binDir)stb_image.o $(binDir)shader.o

.PHONY:  clean

all : run$(EXT)

init: $(shell mkdir ./bin)

run$(EXT) : init main.o $(LINK)
	g++ $(DEFS) -o assign3_part2 main.o $(LINK) $(GL_LIBS) -ggdb3

$(binDir)shader.o : $(utiDir)shader.cpp $(utiDir)shader.hpp
	g++ $(DEFS) -c -o $(binDir)shader.o $(utiDir)shader.cpp -ggdb3

$(binDir)Object.o : $(objectDir)Object.cpp 
	g++ $(DEFS) -c -o $(binDir)Object.o $(objectDir)Object.cpp -ggdb3

$(binDir)Camera.o : $(vieDir)Camera.cpp 
	g++ $(DEFS) -c -o $(binDir)Camera.o $(vieDir)Camera.cpp -ggdb3

$(binDir)helpers.o : $(utiDir)helpers.cpp
	g++ $(DEFS) -c -o $(binDir)helpers.o $(utiDir)helpers.cpp -ggdb3

$(binDir)SkyBox.o : $(skyboxDir)SkyBox.cpp
	g++ $(DEFS) -c -o $(binDir)SkyBox.o $(skyboxDir)SkyBox.cpp -ggdb3

$(binDir)ParticleGenerator.o : $(particleDir)ParticleGenerator.cpp
	g++ $(DEFS) -c -o $(binDir)ParticleGenerator.o $(particleDir)ParticleGenerator.cpp -ggdb3

$(binDir)tiny_obj_loader.o: $(libDir)tiny_obj_loader.cpp
	$(CC) -c -o $(binDir)tiny_obj_loader.o $(libDir)tiny_obj_loader.cpp

$(binDir)stb_image.o: $(libDir)stb_image.cpp
	$(CC) -c -o $(binDir)stb_image.o $(libDir)stb_image.cpp

clean:
	rm -f $(binDir)*.o *.o assign3_part2 $(EXT)
