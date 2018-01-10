#2 - Struttura del codice sorgente

##2.1 - Suddivisione dei file
  
La scelta che è stata fatta per la suddivisione dei file è stata quella di avere:
* 1 file per la definizione della licenza: LICENSE
* 1 file per la gestione dei test: minunit.h
* 2 file per la gestione della compilazione: Makefile e defines.h
* 3 file per la gestione della stampa e lo stile di essa: colors.h e console.h/c
* 5 file per codifica e decodifica di Huffman Adattivo: huffman.h/c, main.c, utilities.h/c

##2.2 - Scopo di ognuno
###2.2.1 - LICENSE

*LICENSE* è un file di testo nel quale viene semplicemente definita la licenza del software.

###2.2.2 - minunit.h

*minunit.h* è un header file contenente le definizioni di 3 funzioni quali:
* mu_assert
* mu_run_test
* mu_tag 

che sono ripettivamente le funzioni di messaggio, esecuzione e etichetta dei test.

###2.2.3 - Makefile
*Makefile* è un file necessario per semplificare la compilazione del codice.

###2.2.4 - defines.h
*defines.h* è un header file contenente vari flags necessari alla compilazione del codice.

###2.2.5 - colors.h
*colors.h* è un header file che definisce stile e colori da utilizzare nella stampa.

###2.2.6 - console.h/c
*colors.h* e *colors.c* sono rispettivamente header file e file di codice contenenti definizioni delle funzioni di stampa a video presenti nel codice.

###2.2.7 - huffman.h/c
*huffman.h* e *huffman.c* sono rispettivamente header file e file di codice contenenti definizioni delle funzione atte alla gestione dell'albero presente nell'algoritmo.

###2.2.7 - main.c
*main.c* è il file principale dove sono definite e implementate le funzioni contenenti le operazioni quali lettura e scrittura su file.

###2.2.8 - utilities.h/c
*utilities.h* e *utilities.c* sono rispettivamente header file e file di codice contenenti funzioni di varia utilità utilizzate nel progetto.