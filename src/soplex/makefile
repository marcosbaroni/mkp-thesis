SPROOT=/home/mbaroni/Downloads/soplex-2.2.0
SPINC=$(SPROOT)/src
SPLIB=$(SPROOT)/lib
CARGS=-O3
all:
	g++ -o sop sop.cpp -I$(SPINC) -L$(SPLIB) $(CARGS) -lsoplex -lz
