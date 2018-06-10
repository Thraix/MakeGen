# This Makefile was generated using MakeGen v1.0.2 made by Tim Håkansson
# and is licensed under MIT. Full source of the project can be found at
# https://github.com/Thraix/MakeGen
CC=@g++
CO=@g++ -o
BIN=bin/
OBJPATH=$(BIN)intermediates
INCLUDES=
OBJECTS=$(OBJPATH)/ConfigFile.o $(OBJPATH)/IncludeDeps.o $(OBJPATH)/Makefile.o $(OBJPATH)/main.o 
CFLAGS=$(INCLUDES) -std=c++17 -c -w -g3 -D_DEBUG 
LIBS=
OUTPUT=$(BIN)makegen
all: $(OUTPUT)
rebuild: clean all
clean:
	$(info Removing intermediates)
	rm -rf $(OBJPATH)/*.o
$(OUTPUT): $(OBJECTS)
	$(info Generating output file)
	$(CO) $(OUTPUT) $(OBJECTS) $(LIBS)
install: all
	$(info Installing MakeGen to /usr/bin/)
	@cp $(OUTPUT) /usr/bin/makegen
$(OBJPATH)/ConfigFile.o : src/ConfigFile.cpp src/Common.h src/ConfigFile.h
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/IncludeDeps.o : src/IncludeDeps.cpp src/IncludeDeps.h
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Makefile.o : src/Makefile.cpp src/Common.h src/IncludeDeps.h src/Makefile.h src/ConfigFile.h
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/main.o : src/main.cpp src/Common.h src/ConfigFile.h src/IncludeDeps.h src/Makefile.h 
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
