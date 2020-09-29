# Paths
DSRC = src
DOBJ = obj
DBIN = bin
 
# Compiler flags
CFLAGS = -g -Wall -Wextra #-D_DEBUG
 
# Commands
CC = g++ -c $(CFLAGS)
LK = g++
RM = del
 
# Project list conv
OBJSC =      \
$(DOBJ)/main.o  \
$(DOBJ)/bmp.o  \

 
#Rebuild all target
all: $(DBIN)/image_ascii

# main target (OBJS + init code + library)
$(DBIN)/image_ascii: $(OBJSC)
	@echo "============================================="
	@echo "image_ascii"
	$(LK) -o $(DBIN)/image_ascii $(OBJSC)
	@echo "============================================="

# objects
 
# The main.cpp file
$(DOBJ)/main.o : $(DSRC)/main.cpp $(DSRC)/bmp/bmp.hpp
	@echo "============================================="
	@echo "main.o"
	$(CC) $(DSRC)/main.cpp -o $(DOBJ)/main.o
	@echo "============================================="


# The bmp.cpp file
$(DOBJ)/bmp.o : $(DSRC)/bmp/bmp.cpp $(DSRC)/bmp/bmp.hpp
	@echo "============================================="
	@echo "bmp.o"
	$(CC) $(DSRC)/bmp/bmp.cpp -o $(DOBJ)/bmp.o
	@echo "============================================="
# Clean all target
clean:
	rm $(DOBJ)/*.o
	rm $(DBIN)/image_ascii