# Nome del compilatore
CC = gcc
# Flag di compilazione (ottimizzazione + warning)
CFLAGS = -O2 -Wall
# Nome dell'eseguibile
EXEC = sph

# File oggetto da compilare
OBJ = main_sph.o IC_sph.o ricerca_vicini.o density_forces.o evolution.o

# Target di default
all: $(EXEC)

# Come compilare l'eseguibile 
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) -lm

# Regole per compilare ogni .c → .o
main_sph.o: main_sph.c physic.h
IC_sph.o: IC_sph.c physic.h
ricerca_vicini.o: ricerca_vicini.c physic.h
density_kernel.o: density_forces.c physic.h
evolution.o: evolution.c physic.h

# Eseguire con file parametri del Sistema Solare
run_sph: $(EXEC)
	./$(EXEC) parameters.dat


# Pulizia dei file generati
clean:
	rm -f $(EXEC) *.o *.txt *.bin *.png
