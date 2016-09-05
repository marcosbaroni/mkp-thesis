SPROOT=/home/mbaroni/Computacao/phd/soplex-2.2.1
SPINC=$(SPROOT)/src
SPLIB=$(SPROOT)/lib
CARGS=-O3
all:
	#g++ -o example example.cpp -I$(SPINC) -L$(SPLIB) $(CARGS) -lsoplex -lz
	g++ -o example example.cpp $(CARGS) -lsoplex -lz -lgmp
