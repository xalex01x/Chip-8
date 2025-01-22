# FastTransport
Si tratta del progetto svolto per il corso a scelta di programmazione avanzata presso il CdL di ingegneria informatica dell'università di Pisa.
Il codice si compone di due progetti distinti, PROGETTO_CLIENT e PROGETTO_SERVER, da inserire come progetti di NetBeans, l'IDE utilizzato nel nostro corso per programmare in Java.
Il server è stato creato avvalendosi del framework Spring.
Il client mette a disposizione un'interfaccia grafica javaFX.

Questo progetto simula il funzionamento di un'applicazione client-server per la gestione di un'azienda di trasporti.
Al database dell'azienda possono accedere due classi di lavoratori: gli admin possono visualizzare tutte le informazioni contenute nel database, possono aggiungere nuovi viaggi, veicoli e sedi logistiche dell'azienda; i worker sono i lavoratori "manuali" del database e non possono fare altro che visualizzare i viaggi che li riguardano attraverso un'apposito calendario.
Punto forte del progetto è il riuscito controllo degli accessi al database che impedisce ai normali worker di visualizzare i dati sensibili degli altri utenti.

Per eseguire il progetto è necessario avere a disposizione NetBeans, javaFX e MySQL.
Avviando l'applicazione server, se MySQL è già attivo, il database di progetto verrà inizializzato automaticamente.