#CXX=g++
CC=armv7l-linux-musleabihf-gcc

SOURCES = $(patsubst src/%, %, $(wildcard src/*.c))
SOURCES += device/main.c device/i_fb_video.c device/i_no_sound.c device/i_no_music.c
# Add orbic-hw files
SOURCES += device/orbic-hw/orbic_fb.c
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))
TARGET_OBJS = $(patsubst %, build/%, $(OBJECTS))
CCFLAGS = -static  -DNORMALUNIX -include stdint.h -std=gnu99   -Wno-implicit-int  
CCFLAGS += -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 -marm
# Patch to find headers
CCFLAGS += -I. -Isrc -Isrc/device -Isrc/device/orbic-hw
.PHONY:
all: doom

.PHONY: sources
sources:
	@echo $(SOURCES)

.PHONY: objects
objects:
	@echo $(TARGET_OBJS)

build:
	mkdir -p build/device/orbic-hw

build/%.o: src/%.c
	@mkdir -p build/device/orbic-hw
	@echo "Compiling $<..."
	@$(CC) -c $< $(CCFLAGS) -g -o $@

build/device/%.o: src/device/%.c
	@echo "Compiling $<..."
	@$(CC) -c $< $(CCFLAGS) -g -o $@

.PHONY:
link: $(TARGET_OBJS)
	@echo "Linking..."
	@$(CC) -static -o doom $(TARGET_OBJS)

doom: link
clean:
	rm -rf ./build/*

