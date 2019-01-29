# This Makefile was generated using MakeGen v1.0.9 made by Tim Håkansson
# and is licensed under MIT. Full source of the project can be found at
# https://github.com/Thraix/MakeGen
CC=@g++
CO=@g++ -o
BIN=bin/
OBJPATH=$(BIN)intermediates
INCLUDES=
OBJECTS=$(OBJPATH)/ConfigFile.o $(OBJPATH)/IncludeDeps.o $(OBJPATH)/Makefile.o $(OBJPATH)/main.o 
CFLAGS=$(INCLUDES) -std=c++17 -c -w -g3 -D_DEBUG 
LIBDIR=
LDFLAGS=
LIBS=$(LIBDIR) 
OUTPUT=$(BIN)makegen
all: $(OUTPUT)
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
$(OBJPATH)/ConfigFile.o : src/ConfigFile.cpp src/Common.h src/ConfigFile.h
	$(info -[25%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/IncludeDeps.o : src/IncludeDeps.cpp src/IncludeDeps.h
	$(info -[50%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Makefile.o : src/Makefile.cpp src/Common.h src/IncludeDeps.h src/Makefile.h src/ConfigFile.h
	$(info -[75%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/main.o : src/main.cpp src/Common.h src/ConfigFile.h src/IncludeDeps.h src/Makefile.h  src/Timer.h
	$(info -[100%]- $<)
	$(CC) $(CFLAGS) -o $@ $<
