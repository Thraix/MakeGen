# This Makefile was generated using MakeGen v1.1.1 made by Tim Håkansson
# and is licensed under MIT. Full source of the project can be found at
# https://github.com/Thraix/MakeGen
CC=@g++
CO=@g++ -o
MKDIR_P=mkdir -p
BIN=bin/
OBJPATH=$(BIN)intermediates
INCLUDES=
OBJECTS=$(OBJPATH)/ConfigFile.o $(OBJPATH)/HFileGen.o $(OBJPATH)/IncludeDeps.o $(OBJPATH)/Makefile.o $(OBJPATH)/main.o 
CFLAGS=$(INCLUDES) -std=c++17 -c -w -g3 -D_DEBUG 
LIBDIR=
LDFLAGS=
LIBS=$(LIBDIR) 
OUTPUT=$(BIN)makegen
.PHONY: all directories rebuild clean dependencies
all: dependencies directories $(OUTPUT)
dependencies:
directories: $(BIN) $(OBJPATH)
$(BIN):
	$(info Creating output directories)
	@$(MKDIR_P) $(BIN)
$(OBJPATH):
	@$(MKDIR_P) $(OBJPATH)
rebuild: clean all
clean:
	$(info Removing intermediates)
	rm -rf $(OBJPATH)/*.o
$(OUTPUT): $(OBJECTS)
	$(info Generating output file)
	$(CO) $(OUTPUT) $(OBJECTS) $(LDFLAGS) $(LIBS)
install: all
	$(info Installing MakeGen to /usr/bin/)
	@cp $(OUTPUT) /usr/bin/makegen
$(OBJPATH)/ConfigFile.o : /home/thraix/Documents/MakeGen/src/ConfigFile.cpp /home/thraix/Documents/MakeGen/src/Common.h /home/thraix/Documents/MakeGen/src/ConfigFile.h /home/thraix/Documents/MakeGen/src/FileUtils.h 
	$(info -[20%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/HFileGen.o : /home/thraix/Documents/MakeGen/src/HFileGen.cpp /home/thraix/Documents/MakeGen/src/FileUtils.h /home/thraix/Documents/MakeGen/src/Common.h /home/thraix/Documents/MakeGen/src/HFileGen.h /home/thraix/Documents/MakeGen/src/ConfigFile.h
	$(info -[40%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/IncludeDeps.o : /home/thraix/Documents/MakeGen/src/IncludeDeps.cpp /home/thraix/Documents/MakeGen/src/Common.h /home/thraix/Documents/MakeGen/src/IncludeDeps.h
	$(info -[60%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Makefile.o : /home/thraix/Documents/MakeGen/src/Makefile.cpp /home/thraix/Documents/MakeGen/src/Common.h /home/thraix/Documents/MakeGen/src/FileUtils.h  /home/thraix/Documents/MakeGen/src/IncludeDeps.h /home/thraix/Documents/MakeGen/src/Makefile.h /home/thraix/Documents/MakeGen/src/ConfigFile.h
	$(info -[80%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/main.o : /home/thraix/Documents/MakeGen/src/main.cpp /home/thraix/Documents/MakeGen/src/Common.h /home/thraix/Documents/MakeGen/src/ConfigFile.h /home/thraix/Documents/MakeGen/src/HFileGen.h  /home/thraix/Documents/MakeGen/src/IncludeDeps.h /home/thraix/Documents/MakeGen/src/Makefile.h  /home/thraix/Documents/MakeGen/src/Timer.h
	$(info -[100%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
