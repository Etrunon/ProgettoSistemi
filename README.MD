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
    INvio di messaggi a tutti per ogni singolo aggiornamento di punteggio

#PROTOCOLLO
    ##HEADER: int IDMittente, int timestamp, int codiceMessaggio

    - 1: Invia risposta (int risposta)
    - 2: Richiesta partecipazione (char* pathFIFO, char* nome, int PID)
    - 3: Invia Informazione Logout Al Server ()
    - 4: Accetta Client ()
    - 5: Rifiuta Client ()
    - 6: Nuovo Giocatore Entrato In Gioco (char* nome, int ID, int punti)
    - 7: Giocatore Uscito Dal Gioco (int ID)
    - 8: Aumenta/Riduci Punto (int ID, int punti)
    - 9: Risposta Corretta/Sbagliata (bool giusto)
    - 10: Invia Domanda (int num1, int num2)
    - 11: Server Spegnimento()


TODO, settare l'ID del server a 0 o valore importante! 