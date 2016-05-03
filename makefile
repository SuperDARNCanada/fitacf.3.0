######### PHONY ##########

.PHONY: all clean testing 

######### VARIABLES #########

CC = gcc

CFLAGS = -g -pedantic -Wall -D_GNU_SOURCE $(INCLUDES) 

INCLUDES = -Iinclude/ -I$(IPATH)/superdarn/ -I$(IPATH)/base/ -I$(IPATH)/general/

OBJECTDIR = objects/

SRCDIR = src/

LIBDIR = lib/

BINDIR = bin/

STATIC_LFLAGS = -Wl,-Bstatic $(LIBDIR)libfitacf.a

DYNAMIC_LFLAGS = -Wl,-Bdynamic -lpthread -lm

MFLAGS = -L$(LIBDIR) 

LIBS =  $(MFLAGS) $(STATIC_LFLAGS) $(DYNAMIC_LFLAGS)

######### WHAT TO COMPILE #########

all: fitacf make_fit


######### FITACF LIBRARY ##########
FITACF_TARGET = libfitacf.a

FITACF_SRC = $(shell echo src/*.c)

FITACF_OBJ = $(FITACF_SRC:.c=.o)

fitacf:$(FITACF_OBJ)
	mkdir -p $(LIBDIR)
	ar -rc $(FITACF_TARGET) $(FITACF_OBJ)
	ranlib $(FITACF_TARGET)
	mv $(FITACF_TARGET) $(LIBDIR)


######### TESTING ##########
TESTING = testing_fitacf

TESTING_SRC = $(shell echo testing/*.c)

TESTING_OBJ = $(TESTING_SRC:.c=.o)

testing: $(TESTING_OBJ)
		$(CC) $(CFLAGS) $(INLUDES) -o $(TESTING) $(TESTING_OBJ) $(LIBS)

######### MAKE_FIT #########
MAKEFIT = make_fit

MAKEFIT_SRC = $(shell echo make_fitacf.3.0/*.c)

MAKEFIT_OBJ = $(MAKEFIT_SRC:.c=.o)

MK_STATIC_LFLAGS = -Wl,-Bstatic $(LIBDIR)libfitacf.a -loldraw.1 -loldfit.1 -lcfit.1 -lrscan.1 -lradar.1 -lfitacf.1 -lraw.1 -lfit.1 -ldmap.1 -lopt.1 -lrtime.1 -lrcnv.1 

MK_DYNAMIC_LFLAGS = -Wl,-Bdynamic -lpthread -lm -lz

MFLAGS = -L$(LIBDIR) -L$(LIBPATH)

MK_LIBS =  $(MFLAGS) $(MK_STATIC_LFLAGS) $(MK_DYNAMIC_LFLAGS)

make_fit: $(MAKEFIT_OBJ)
		$(CC) $(CFLAGS) $(INLUDES) -o $(MAKEFIT) $(MAKEFIT_OBJ) $(MK_LIBS)
		mkdir -p $(BINDIR)
		mv $(MAKEFIT) $(BINDIR)


######### CLEAN #########
clean: 
	rm -rf *.h~ *.c~ *.o *.o~ *.a~ *.exe *.exe.stackdump Makefile~ fullrangeinfo.txt phases.txt
	rm -rf *.*.gch *.txt~  $(TESTING) $(MAKEFIT) $(LIBDIR)$(FITACF_TARGET) $(FITACF_OBJ) $(TESTING_OBJ) $(MAKEFIT_OBJ)




