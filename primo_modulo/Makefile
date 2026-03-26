# Nome del compilatore
CC = gcc
# Flag di compilazione (ottimizzazione + warning)
CFLAGS = -O2 -Wall
# Nome dell'eseguibile
EXEC = orbit

# File oggetto da compilare
OBJ = orbit.o read_func.o integr_func.o conversion.o 

# Target di default
all: $(EXEC)

# Come compilare l'eseguibile 
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) -lm

# Regole per compilare ogni .c → .o
orbit.o: orbit.c read_func.h integr_func.h conversion.h
read_func.o: read_func.c read_func.h
integr_func.o: integr_func.c integr_func.h 
conversion.o: conversion.c conversion.h integr_func.h

# Eseguire con file parametri del Sistema Solare
run_solare: $(EXEC)
	./$(EXEC) parameters_sun.dat

# Eseguire con file parametri di TRAPPIST
run_trappist: $(EXEC)
	./$(EXEC) parameters_trap.dat

# Pulizia dei file generati
clean:
	rm -f $(EXEC) *.o *.txt *.bin
