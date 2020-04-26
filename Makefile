# This Makefile was generated using MakeGen v1.3.1 made by Tim Håkansson
# and is licensed under MIT. Full source of the project can be found at
# https://github.com/Thraix/MakeGen
CC=@g++
CO=@g++ -o
MKDIR_P=mkdir -p
BIN=bin/
OBJPATH=$(BIN)intermediates
INCLUDES=
OBJECTS=$(OBJPATH)/ConfigCLI.o $(OBJPATH)/ConfigFile.o $(OBJPATH)/HFileGen.o $(OBJPATH)/IncludeDeps.o $(OBJPATH)/Makefile.o $(OBJPATH)/Utils.o $(OBJPATH)/ConfigFileConf.o $(OBJPATH)/main.o $(OBJPATH)/XML.o $(OBJPATH)/XMLObject.o 
CFLAGS=$(INCLUDES) -std=c++17 -c 
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
$(OBJPATH)/ConfigCLI.o : src/ConfigCLI.cpp src/Common.h src/ConfigCLI.h src/ConfigFile.h src/ConfigUtils.h  src/FileUtils.h  src/Utils.h src/xml/XMLObject.h 
	$(info -[10%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/ConfigFile.o : src/ConfigFile.cpp src/ConfigFile.h src/ConfigUtils.h src/Common.h src/FileUtils.h  src/Utils.h src/xml/XMLObject.h  src/compatibility/ConfigFileConf.h src/xml/XML.h 
	$(info -[20%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/HFileGen.o : src/HFileGen.cpp src/FileUtils.h src/Common.h src/Utils.h src/HFileGen.h src/ConfigFile.h src/ConfigUtils.h   src/xml/XMLObject.h
	$(info -[30%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/IncludeDeps.o : src/IncludeDeps.cpp src/Common.h src/IncludeDeps.h src/ConfigFile.h src/ConfigUtils.h  src/FileUtils.h  src/Utils.h src/xml/XMLObject.h 
	$(info -[40%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Makefile.o : src/Makefile.cpp src/IncludeDeps.h src/ConfigFile.h src/ConfigUtils.h src/Common.h src/FileUtils.h  src/Utils.h src/xml/XMLObject.h  src/Makefile.h  
	$(info -[50%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Utils.o : src/Utils.cpp src/ConfigFile.h src/ConfigUtils.h src/Common.h src/FileUtils.h  src/Utils.h src/xml/XMLObject.h  
	$(info -[60%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/ConfigFileConf.o : src/compatibility/ConfigFileConf.cpp src/ConfigFile.h src/ConfigUtils.h src/Common.h src/FileUtils.h  src/Utils.h src/xml/XMLObject.h  src/compatibility/ConfigFileConf.h
	$(info -[70%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/main.o : src/main.cpp src/Common.h src/ConfigCLI.h src/ConfigFile.h src/ConfigUtils.h  src/FileUtils.h  src/Utils.h src/xml/XMLObject.h   src/HFileGen.h  src/Makefile.h  src/Timer.h
	$(info -[80%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/XML.o : src/xml/XML.cpp src/xml/XML.h src/xml/XMLObject.h src/xml/XMLException.h 
	$(info -[90%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/XMLObject.o : src/xml/XMLObject.cpp src/Common.h src/Utils.h src/xml/XMLException.h src/xml/XMLObject.h 
	$(info -[100%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
