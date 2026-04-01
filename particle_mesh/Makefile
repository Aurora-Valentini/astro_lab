# Percorsi trovati con brew --prefix
FFTW_PATH = /usr/local

# 2. Flag per il compilatore (Headers) e Linker (Librerie)
IFLAGS = -I$(FFTW_PATH)/include
LDFLAGS = -L$(FFTW_PATH)/lib -lfftw3 -lm

# Nome del compilatore
CC = gcc
# Flag di compilazione (ottimizzazione + warning)
CFLAGS = -O3 -Wall $(IFLAGS)
# Nome dell'eseguibile
EXEC = main_pm

# File oggetto da compilare
OBJ = main_pm.o initial_cond.o density_comp.o potential_comp.o evolution.o

# Target di default (se non specificato compilo con NGP)
all: ngp

# Target per scegliere uno dei 3 metodi per assegnare la massa 

ngp: CFLAGS += -DNGP
ngp: $(EXEC)

cic: CFLAGS += -DCIC
cic: $(EXEC)

tsc: CFLAGS += -DTSC
tsc: $(EXEC)

# Come compilare l'eseguibile 
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) $(LDFLAGS) 

# Regole per compilare ogni .c → .o
main_pm.o: main_pm.c physic.h
	$(CC) $(CFLAGS) -c main_pm.c

initial_cond.o: initial_cond.c physic.h
	$(CC) $(CFLAGS) -c initial_cond.c

density_comp.o: density_comp.c physic.h
	$(CC) $(CFLAGS) -c density_comp.c

potential_comp.o: potential_comp.c physic.h
	$(CC) $(CFLAGS) -c potential_comp.c

evolution.o: evolution.c physic.h
	$(CC) $(CFLAGS) -c evolution.c

# Eseguire con file parametri
run_pm: $(EXEC)
	./$(EXEC) parameters.dat


# Pulizia dei file generati
clean:
	rm -f $(EXEC) *.o *.txt *.bin
