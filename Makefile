#lista delle librerie necessarie
OBJ = main.o server.o client.o allFifo.o commands.o logica.o gui.o guiMessages.o riparser.o
#lista delle librerie da testare
OBJTEST = allFifo.o parser.o TestGui.o logica.o gui.o guiMessages.o

#Definiamo quali direttive non producono file
.PHONY: clean

#path in cui cercare i file sorgenti e binari per verificare che siano presenti
VPATH = bin:src

#path in cui ci sono i sorgenti
SRCDIR = src

#nome e posizione dell'eseguibile
BUILDNAME = main
BUILDIR = bin
CFLAGS = -c -g #-DDEBUGFIFO #-std=c99

all:
	@printf "%s\n" "Target Make disponibili:"
	@printf "\t%s\t\t%s\n" "bin" "compila i sorgenti"
	@printf "\t%s\t\t%s\n" "assets" "genera i file di test"
	@printf "\t%s\t\t%s\n" "test" "avvia il testing"
	@printf "\t%s\t\t%s\n" "clean" "elimina i file generati dalla compilazione o testing"

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
