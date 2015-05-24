#lista delle librerie necessarie
OBJ = main.o server.o client.o allFifo.o commands.o logica.o gui.o guiMessages.o parser.o messaggiASchermo.o

#lista delle librerie da comilare, elencate qui per praticità durante lo sviluppo
OBJTEST = allFifo.o parser.o logica.o gui.o guiMessages.o TestGui.o

#lista usata per avviare più client durante il testing
NUMBERS = 0 1 2 3 4 5 6 7 8 9

#Definiamo quali direttive non producono file
.PHONY: all clean test

#path in cui cercare i file sorgenti e binari, necesario per make
VPATH = bin:src

#path in cui ci sono i sorgenti
SRCDIR = src

#nome e posizione dell'eseguibile
BUILDNAME = main
BUILDIR = bin

#flag da usare per compilare il programma
CFLAGS = -c -pthread -std=c99 -w

#spiegazione dei comandi make disponibili
all:
	@printf "%s\n" "Target Make disponibili:"
	@printf "\t%s\t\t%s\n" "bin" "compila i sorgenti"
	@printf "\t%s\t\t%s\n" "assets" "genera i file di test"
	@printf "\t%s\t\t%s\n" "test" "avvia il testing"
	@printf "\t%s\t\t%s\n" "clean" "elimina i file generati dalla compilazione o testing"

#compila tutti le librerie e le linka, creando un unico eseguibile
bin: clean checkDir $(BUILDNAME)

#cancella qualunque pezzo di codice generato, rimuovendo la cartella dei binari e degli assets
clean:
	-@rm -rf $(BUILDIR)
	-@rm -rf assets
	@printf "%s\n" "File binari e assets compilati precedentemente sono stati eliminati"


#compila il file sorgente dedicato alla creazione di assets, lo esegue e lo cancella
assets: clean
	@-mkdir -p assets
	@gcc $(SRCDIR)/assetsGenerator.c -o assets/assetsGenerator -g
	@(./assets/assetsGenerator 5)
	@-rm -rf assets/assetsGenerator

#fa partire il testing automatico, redirezionando l'input dei client da file
test: assets bin
	@(./$(BUILDIR)/$(BUILDNAME) --server --max 10 --win 20 --testing --seed 5 &)
	@$(foreach var,$(NUMBERS),./$(BUILDIR)/$(BUILDNAME) --client --testing < ./assets/ClientFile$(var).txt &)

#controlla che esista la cartella in cui inserire i compilati
checkDir:
	-@mkdir -p $(BUILDIR)

#compila tutti i file *.c in file *.o nella cartella build/
%.o: %.c
	@gcc $< -o $(BUILDIR)/$@ $(CFLAGS)

#target utilizzato durante lo sviluppo
$(BUILDNAME): $(OBJ)
	@#linking finale
	@gcc -pthread $(addprefix $(BUILDIR)/,$(^F)) -o $(BUILDIR)/$@
	@#copia l'immagine contenente il logo del gioco in una posizione assoluta perché sia sempre raggiungibile dal programma
	@-cp -f src/art.txt /tmp/art.txt
	@printf "%s\n" "Compilazione terminata correttamente"
	@#-rm $(addprefix $(BUILDIR)/,$(^F))

#target usato durante lo sviluppo
BUILDTESTLIB: checkDir buildtest

#target usato durante lo sviluppo
buildtest: $(OBJTEST)
	@#linking finale
	@gcc -pthread $(addprefix $(BUILDIR)/,$(^F)) -o $(BUILDIR)/$(BUILDNAME)
	@#-rm $(addprefix $(BUILDIR)/,$(^F))
