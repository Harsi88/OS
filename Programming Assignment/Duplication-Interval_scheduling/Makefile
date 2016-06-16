SYSTEM = x86-64_sles10_4.1
CCC = g++ -g	
CCOPT = -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD

#SYSTEM = x86_sles10_4.1
#CCC = g++ -O0 -g
#CCOPT = -m32 -O -fPIC -fexceptions -DNDEBUG -DIL_STD

LIBFORMAT  =static_pic

CPDIR      = /opt/ibm/ILOG/CPLEX_Studio125/cpoptimizer
CPLIBDIR   = $(CPDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio125/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio125/concert

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLIBDIR) -lcp -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CPINCDIR	  = $(CPDIR)/include

CCFLAGS = $(CCOPT) -I$(CPINCDIR) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) 


#all: nodupcp

OBJS = processor.o schedule.o nodupcont.o main_nodup.o
OBJSD = processor.o schedule.o dupcont.o main.o
#OBJSDN = schedule.o dupcontn.o main.o
  

all: CONT CONTD #CONTDN

CONT : $(OBJS)
	$(CCC) $(CCFLAGS) -o CONT $(OBJS) $(CCLNFLAGS)

CONTD : $(OBJSD)
	$(CCC) $(CCFLAGS) -o CONTD $(OBJSD) $(CCLNFLAGS)

CONTDN : $(OBJSDN)
	$(CCC) $(CCFLAGS) -o CONTDN $(OBJSDN) $(CCLNFLAGS)

processor.o: processor.cpp
	$(CCC) -c processor.cpp

schedule.o: schedule.cpp
	$(CCC) -c schedule.cpp

nodupcp: schedule.o processor.o
	$(CCC) -c $(CCFLAGS) no_duplication_cp.cpp -o no_duplication_cp.o
	$(CCC) $(CCFLAGS) processor.o schedule.o no_duplication_cp.o main_nodup.cpp -o nodupcp.out $(CCLNFLAGS)	

nodupcont.o: no_duplication_contention.cpp schedule.h
	$(CCC) -c $(CCFLAGS) no_duplication_contention.cpp -o nodupcont.o

dupcont.o: general_duplication_cont.cpp schedule.h
	$(CCC) -c $(CCFLAGS) general_duplication_cont.cpp -o dupcont.o

dupcontn.o: general_duplication_cont_new.cpp schedule.h
	$(CCC) -c $(CCFLAGS) general_duplication_cont_new.cpp -o dupcontn.o

main_nodup.o: main_nodup.cpp
	$(CCC) -c main_nodup.cpp	
	
main.o: main.cpp
	$(CCC) -c main.cpp
	
clean:
	rm -rf *.o CONT* *.out
	
