# This Makefile was generated using MakeGen v1.1.7 made by Tim Håkansson
# and is licensed under MIT. Full source of the project can be found at
# https://github.com/Thraix/MakeGen
CC=@g++
CO=@g++ -o
MKDIR_P=mkdir -p
BIN=bin/
OBJPATH=$(BIN)intermediates
INCLUDES=
OBJECTS=$(OBJPATH)/ConfigFile.o $(OBJPATH)/HFileGen.o $(OBJPATH)/IncludeDeps.o $(OBJPATH)/Makefile.o $(OBJPATH)/Utils.o $(OBJPATH)/main.o 
CFLAGS=$(INCLUDES) -std=c++17 -c -w -g3 -D_DEBUG 
LIBDIR=
LDFLAGS=
LIBS=$(LIBDIR) 
OUTPUT=$(BIN)makegen
.PHONY: all directories rebuild clean run
all: directories $(OUTPUT)
directories: $(BIN) $(OBJPATH)
$(BIN):
	$(info Creating output directories)
	@$(MKDIR_P) $(BIN)
$(OBJPATH):
	@$(MKDIR_P) $(OBJPATH)
run: all
	@./$(OUTPUT)
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
$(OBJPATH)/ConfigFile.o : src/ConfigFile.cpp src/Common.h src/ConfigFile.h src/FileUtils.h  src/Utils.h 
	$(info -[16%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/HFileGen.o : src/HFileGen.cpp src/FileUtils.h src/Common.h src/Utils.h src/ConfigFile.h src/HFileGen.h 
	$(info -[33%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/IncludeDeps.o : src/IncludeDeps.cpp src/Common.h src/IncludeDeps.h src/ConfigFile.h src/FileUtils.h  src/Utils.h  
	$(info -[50%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Makefile.o : src/Makefile.cpp src/Common.h src/FileUtils.h  src/Utils.h src/ConfigFile.h src/IncludeDeps.h    src/Makefile.h  
	$(info -[66%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Utils.o : src/Utils.cpp src/FileUtils.h src/Common.h src/Utils.h src/ConfigFile.h 
	$(info -[83%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/main.o : src/main.cpp src/Common.h src/ConfigFile.h src/FileUtils.h  src/Utils.h  src/HFileGen.h  src/IncludeDeps.h    src/Makefile.h  src/Timer.h
	$(info -[100%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
