#lista delle librerie necessarie
OBJ = main.o server.o client.o allFifo.o commands.o logica.o gui.o guiMessages.o riparser.o
#lista delle librerie da testare
OBJTEST = allFifo.o riparser.o logica.o gui.o guiMessages.o mainBoss.o

NUMBERS = 0 1 2 3
#Definiamo quali direttive non producono file
.PHONY: clean test

#path in cui cercare i file sorgenti e binari per verificare che siano presenti
VPATH = bin:src

#path in cui ci sono i sorgenti
SRCDIR = src

#nome e posizione dell'eseguibile
BUILDNAME = main
BUILDIR = bin
CFLAGS = -c -g -pthread -std=c99 -DDEBUGFIFO

all:
	@printf "%s\n" "Target Make disponibili:"
	@printf "\t%s\t\t%s\n" "bin" "compila i sorgenti"
	@printf "\t%s\t\t%s\n" "assets" "genera i file di test"
	@printf "\t%s\t\t%s\n" "test" "avvia il testing"
	@printf "\t%s\t\t%s\n" "clean" "elimina i file generati dalla compilazione o testing"

bin: checkDir $(BUILDNAME)

clean:
	-@rm -rf $(BUILDIR)
	-@rm -rf assets

assets:
	@#compila il file sorgente dedicato alla creazione di assets, lo esegue e lo cancella
	@-mkdir -p assets
	@gcc $(SRCDIR)/assetsGenerator.c -o assets/assetsGenerator -g
	@./assets/assetsGenerator
	@-rm -rf assets/assetsGenerator

test: assets bin
	#fa partire il testing
	@(./$(BUILDIR)/$(BUILDNAME) --server --testing &)
	@$(foreach var,$(NUMBERS),./$(BUILDIR)/$(BUILDNAME) --client --testing < ./assets/ClientFile$(var).txt &)


checkDir:
	-@mkdir -p $(BUILDIR)

#compila tutti i file *.c in file *.o nella cartella build/
%.o: %.c
	@gcc $< -o $(BUILDIR)/$@ $(CFLAGS)

$(BUILDNAME): $(OBJ)
	@#linking finale
	@gcc -pthread $(addprefix $(BUILDIR)/,$(^F)) -o $(BUILDIR)/$@
	@-cp src/art.txt /tmp/art.txt
	@#-rm $(addprefix $(BUILDIR)/,$(^F))


BUILDTESTLIB: checkDir buildtest

buildtest: $(OBJTEST)
	@#linking finale
	@gcc -pthread $(addprefix $(BUILDIR)/,$(^F)) -o $(BUILDIR)/$(BUILDNAME)
	@#-rm $(addprefix $(BUILDIR)/,$(^F))
