GUESSTHENUMBER GAME!
===
```
 __          ___                         __         
/ _     _ _ _ | |_  _|\ |    _ |_  _ _  / _  _  _  _
\__)|_|(-_)_) | | )(-| \||_|||||_)(-|   \__)(_||||(-
```

Sviluppato da Luca Bosotti e Carlo Mion per il progetto di laboratorio di Sistemi Operativi nell'A.A 2014/2015

## MODULI
* Parti comuni
    - "PARSER" messaggi. Prende/Crea stringa vuota, intero significato messaggio, più parametri necessari. Formatta il messaggio coerentemente e
    
    - Gestione signal di interruzione
    - GUI

* Server
    - Controllo unicità
    - Gestione I/O giocatori

* Client
    - Signal inviato da server deve avvisare a schermo che non c'è più server
    - modulo interazione utente

* Logica Gioco
    - Modulo che gestisce la partita
    - Generatore domande
    - Gestione classifica (array di nomi e punteggi che viene aggiornato dai vari messaggi ricevuti)


## INTERFACCIA FIFO
* Per scrivere: puntatore a messaggio già fatto, con i vari campi compilati + FIFO in cui mandare;
Parser distrugge messaggio
* Per leggere: puntatore nullo a messaggio da riempire;
Server/Client distrugge messaggio

## CLASSIFICA
    Invio di messaggi a tutti per ogni singolo aggiornamento di punteggio

## PROTOCOLLO
HEADER: int IDMittente, int timestamp, int codiceMessaggio
- 0 Invia risposta (int risposta)
- 1 Richiesta partecipazione (char* pathFIFO, char* nome, int PID (preso dal riparser)
- 2 Invia Informazione Logout Al Server ()
- 3 Accetta Client (int ID, int punti, int maxWin)
- 4 Rifiuta Client ()
- 5 Nuovo Giocatore Entrato In Gioco (char* nome, int ID, int punti)
- 6 Giocatore Uscito Dal Gioco (int ID)
- 7 Modifica Punti giocatore(int ID, int punti, bool giusto)
- 8 Esito Risposta (bool giusto, int punti)
- 9 Invia Domanda (int num1, int num2)
- 10 Server Spegnimento()
- 11 Vittoria(int ID)

## BUG
- punteggio in classifica a -1 (trololol)
- nome in classifica sempre uguale