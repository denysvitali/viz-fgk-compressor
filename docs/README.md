# Struttura del codice sorgente

## Suddivisione dei file
  
La scelta che è stata fatta per la suddivisione dei file è stata quella di avere:
* 1 file per la definizione della licenza: LICENSE
* 1 file per la gestione dei test: minunit.h
* 2 file per la gestione della compilazione: Makefile e defines.h
* 3 file per la gestione della stampa e lo stile di essa: colors.h e console.h/c
* 5 file per codifica e decodifica di Huffman Adattivo: huffman.h/c, main.c, utilities.h/c

## Scopo di ogni file
### LICENSE

*LICENSE* è un file di testo nel quale viene semplicemente definita la licenza del software.

### minunit.h

*minunit.h* è un minimal unit testing framework basato su [MinUnit](http://www.jera.com/techinfo/jtns/jtn002.html). Si differenzia dall'originale semplicemente per l'integrazione di tags per i test.  
Il file contiene le seguenti funzioni:
* mu_assert
* mu_run_test
* mu_tag 

che sono ripettivamente le funzioni di messaggio, esecuzione ed etichettamento dei test.

### Makefile
*Makefile* è un file necessario per semplificare la compilazione del codice.

### defines.h
*defines.h* è un header file contenente vari flags necessari alla compilazione del codice quali DEBUG / TEST / RELEASE mode, versione, fallback per commit versioning. Sono inoltre presenti scelte arbitrarie di progetto (come il NYT number ed il Magic Number).

### colors.h
*colors.h* è un header file che definisce stile e colori da utilizzare nella stampa.

### console.{c,h}
*colors.h* e *colors.c* sono rispettivamente header file e file di codice contenenti definizioni delle funzioni di stampa a video presenti nel codice.

### huffman.{c,h}
*huffman.h* e *huffman.c* sono rispettivamente header file e file di codice contenenti definizioni delle funzione atte alla gestione dell'albero presente nell'algoritmo.

### main.c
*main.c* è il file principale dove sono definite e implementate le funzioni contenenti le operazioni quali lettura e scrittura su file.

### utilities.{c,h}
*utilities.h* e *utilities.c* sono rispettivamente header file e file di codice contenenti funzioni di varia utilità utilizzate nel progetto.

# Strutture dati utilizzate

## Node

*Node* è una `struct` definita come segue:
```c
typedef struct Node{    
    int node_number;
    int weight;
    int element;
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;
``` 
La quale identifica ogni nodo dell'albero con i rispettivi attributi (*node_number*, *weight* e *element*).
Sono inoltre presenti i puntatori ai vari nodi limitrofi quali il *parent* e i *child* 
## HuffmanTree

*HuffmanTree* è una `struct` definita come segue:
```c
typedef struct{
    Node* root;
    Node* tree[HUFFMAN_ARRAY_SIZE]; // 514
    Node* nyt;
    char* output;
    char* partial_output;
    int output_length;
    int partial_output_length;
    int elements;
    int decoder_flags;
    unsigned int mode;
    unsigned char mask;
} HuffmanTree;
``` 
La quale identifica l'albero con i suoi attributi(*root* e *nyt*).
Contiene inoltre un `array` utilizzato per avere un accesso più veloce ai nodi dell'albero ed il rispettivo numero (*tree* e *elements*).
Le componenti: *output*, *partial_output* ,*output_length*, *partial_output_length* sono necessarie per le stampe su file e a video.
Le componenti *decoder_flags* e *mode* sono necessarie per decidere le varie flags per le stampe utili al DEBUG e la modalità di utilizzo del software (compressione o decompressione).
La componente *mask* è necessaria per effettuare un padding necessario agli ultimi bit per raggiungere il byte.

# Istruzioni per l'utilizzo

## Compilazione

Per la compilazione del codice è necessario utilizzare la seguente riga di codice:
`make release` 
che genera un file binario dal nome `viz-release`.
Una guida simile è presente nel file *README*

## Esecuzione

### Compressione
Per l'esecuzione del software in compressione è necessario utilizzare la seguente riga di codice:
`./viz-release -c input output.viz`
dove l'argomento `-c` serve per eseguire il programma in compressione, `input` è un file di input con una qualsiasi estensione e `output.viz` è il nome del file che si vorrà avere in output.
**Nota:** Il nome del file in uscita può variare ma non la sua estensione.

### Decompressione
Per l'esecuzione del software in decompressione è necessario utilizzare la seguente riga di codice:
`./viz-release -d input.viz`
dove l'argomento `-d` serve per eseguire il programma in decompressione e `input.viz` è un file di input qualsiasi con estensione `.viz`.
<!--
<div align="center">

![Jenkins](./images/god.png)
# Y'all mind if I praise the Lord?
</div>
-->
# Procedure di test e problemi noti
## Test effettuati
### Test_debug
Testa se il sowftare è in modalità `DEBUG`.

### Test_create_huffman_tree
Testa se la funzione `create_huffman_tree` viene eseguita correttamente.

### Test_swap_ht_array
Testa la funzione `swap_nodes` e confronta il risultato con quello aspettato.

### Test_huffman_coding_bookkeeper
Testa la creazione e l'adattamento dell'albero sulla parola *bookkeeper* e confronta il risultato con quello aspettato.
### Test_huffman_coding_mississippi
Testa la creazione e l'adattamento dell'albero sulla parola *mississippi* e confronta il risultato con quello aspettato.
### Test_huffman_coding_engineering
Testa la creazione e l'adattamento dell'albero sulla parola *engineering* e confronta il risultato con quello aspettato.
### Test_huffman_coding_foobar
Testa la creazione e l'adattamento dell'albero sulla parola *foobar* e confronta il risultato con quello aspettato.
### Test_huffman_coding_foobar2000
Testa la creazione e l'adattamento dell'albero sulla parola *foobar2000* e confronta il risultato con quello aspettato.
### Test_huffman_coding_loremipsum
Testa la creazione e l'adattamento dell'albero sulla parola *loremipsum* e confronta il risultato con quello aspettato.

### Test_get_node_level
Testa la funzione `getNodeLevel` e confronta il risultato con quello aspettato.
### Test_simple_swap
Testa la funzione `swap_node` applicandola ad un esempio semplice e confronta il risultato con quello aspettato.
### Test_swap_nodes
Testa la funzione `swap_node` in tutte le sue funzioni e confronta il risultato con quello aspettato.
### Test_node_path
Testa la funzione `node_path` e confronta il risultato con quello aspettato.
### Test_huffman_coding
Testa l'intero sowftare e confronta il risultato con quello aspettato.
### Test_utility_sibilings
Testa la funzione `sibiling` su un esempio creato.
    //mu_run_test(test_utility_siblings);
    mu_run_test(test_huffman_coding);
    mu_run_test(test_huffman_coding_abracadabra);
    mu_run_test(test_huffman_coding_abcbaaa);
    mu_run_test(test_huffman_coding_mississippi);
    mu_run_test(test_huffman_coding_engineering);
    mu_run_test(test_huffman_coding_aardvark);
    mu_run_test(test_huffman_coding_sleeplessness);
    mu_run_test(test_bin2byte);
    mu_run_test(test_bin2byte2);
    mu_run_test(test_byte2bin);
    mu_run_test(test_filename);


    // File ops. Run in sequence!
    mu_run_test(test_create_file);
    mu_run_test(test_write_to_file);
    mu_run_test(test_read_file);
    mu_run_test(test_file_delete);
