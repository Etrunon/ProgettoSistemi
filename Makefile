#lista delle librerie necessarie
OBJ = creaFifoMain.o creaFifo.o
 #main.o

#Definiamo quali direttive non producono file
.PHONY: clean

#path in cui cercare i file sorgenti e binari per verificare che siano presenti
VPATH = bin:src

#path in cui ci sono i sorgenti
SRCDIR = src

#nome e posizione dell'eseguibile
BUILDNAME = main
BUILDIR = bin

bin: checkDir $(BUILDNAME)

clean:
	-@rm -rf $(BUILDIR)

assets:
	#complia il file sorgente dedicato alla creazione di assets, lo esegue e lo cancella

test: assets bin
	#fa partire il testing
	
checkDir:
	-@mkdir -p $(BUILDIR)

#compila tutti i file *.c in file *.o nella cartella build/
%.o: %.c
	gcc $< -o $(BUILDIR)/$@ -c

$(BUILDNAME): $(OBJ)
	#linking finale
	gcc $(addprefix $(BUILDIR)/,$(^F)) -o $(BUILDIR)/$@
	rm $(addprefix $(BUILDIR)/,$(^F))