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
	$(info ------------------------)
	$(info ---- Done Compiling ----)
	$(info ------------------------)
rebuid: clean all
clean:
	$(info Removing intermediates)
	rm -rf $(OBJPATH)/*.o
$(OUTPUT): $(OBJECTS)
	$(info Generating output file)
	$(CO) $(OUTPUT) $(OBJECTS) $(LIBS)
install: all
	cp $(OUTPUT) /usr/bin/makegen
$(OBJPATH)/ConfigFile.o : src/ConfigFile.cpp src/ConfigFile.h src/Logging.h
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/IncludeDeps.o : src/IncludeDeps.cpp src/IncludeDeps.h
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/Makefile.o : src/Makefile.cpp src/IncludeDeps.h src/Logging.h src/Makefile.h src/ConfigFile.h
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
$(OBJPATH)/main.o : src/main.cpp src/ConfigFile.h src/IncludeDeps.h src/Logging.h src/Makefile.h 
	$(info ---- $<)
	$(CC) $(CFLAGS) -o $@ $<
