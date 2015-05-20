Togliere il file delle cose da fare;

- [ ] Parti comuni
    - [x] L "PARSER" messaggi. Prende/Crea stringa vuota, intero significato messaggio, più parametri necessari. Formatta il messaggio coerentemente e
    - [ ] gestione classifica (array di nomi e punteggi che viene aggiornato dai vari messaggi ricevuti)
    - [ ] C Gestione errori FIFO
    - [ ] Modulo per la gestione degli errori della scrittura tramite parser e fifo
- [ ] Server
    - [x] C Controllo unicità
    - [ ] gestione signal
    - [ ] gestione I/O giocatori
    - [ ] interfaccia

- [ ] Client
    - [ ] gestione signal
        - [] Se signal inviato da server, deve avvisare a schermo che non c'è più server
        - [] Se signal inviato da esterno, deve avvisare server che se ne è andato
    - [x] modulo interazione utente
    - [ ] interfaccia

- [ ] Logica Gioco
    - [ ] modulo che gestisce la partita
        - [ ] check domande
    - [ ] generatore domande


#INTERFACCIA DI FIFO
    * Per scrivere: puntatore a messaggio già fatto, con i vari campi compilati + FIFO in cui mandare;
        Parser distrugge messaggio
    * Per leggere: puntatore nullo a messaggio da riempire;
        Server/client distrugge messaggio

#CLASSIFICA
    Invio di messaggi a tutti per ogni singolo aggiornamento di punteggio

#PROTOCOLLO
    ##HEADER: int IDMittente, int timestamp, int codiceMessaggio

    - 0: Invia risposta (int risposta)
    - 1: Richiesta partecipazione (char* pathFIFO, char* nome, int PID (preso dal riparser)
    - 2: Invia Informazione Logout Al Server ()
    - 3: Accetta Client (int ID, int punti, int maxWin)
    - 4: Rifiuta Client ()
    - 5: Nuovo Giocatore Entrato In Gioco (char* nome, int ID, int punti)
    - 6: Giocatore Uscito Dal Gioco (int ID)
    - 7: Modifica Punti giocatore(int ID, int punti, bool giusto)
    - 8: Esito Risposta (bool giusto, int punti)
    - 9: Invia Domanda (int num1, int num2)
    - 10: Server Spegnimento()
    - 11: Vittoria(int ID)


TODO, settare l'ID del server a 0 o valore importante!
