COMPILER_PATH = clang++

# Optionen
CFLAGS   = -Wall -lb15fdrv

# Objekte
SENDER_OBJECTS = Sender.o
EMPF_OBJECTS   = Empfaenger.o

# Ausgabedateien
SENDER_OUT     = sender.elf
EMPF_OUT       = empfaenger.elf

# Kompilierungsregel
COMPILE = $(COMPILER_PATH) $(CFLAGS)

all: sender empfaenger

# Ziel für den Sender
sender: $(SENDER_OBJECTS)
	$(COMPILE) -o $(SENDER_OUT) $(SENDER_OBJECTS)

# Ziel für den Empfänger
empfaenger: $(EMPF_OBJECTS)
	$(COMPILE) -o $(EMPF_OUT) $(EMPF_OBJECTS)

# Regeln für die Objektdateien
%.o: %.cpp
	$(COMPILE) -c $< -o $@

# Aufräumen
clean:
	rm -f *.o $(SENDER_OUT) $(EMPF_OUT)
