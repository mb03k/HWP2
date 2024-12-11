COMPILER_PATH = g++

# Optionen
CFLAGS   = -std=c++17 -O3 -Wall -Wextra

# Objekte
SENDER_OBJECTS = sender_neu.o
EMPF_OBJECTS   = empfaenger.o

# Ausgabedateien
SENDER_OUT     = sender.elf
EMPF_OUT       = empfaenger.elf

# Kompilierungsregel
COMPILE = $(COMPILER_PATH) $(CFLAGS)

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
