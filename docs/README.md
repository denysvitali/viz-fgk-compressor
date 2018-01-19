# Struttura del codice sorgente  

## Introduzione

### Progetto  
Il presente progetto è stato sviluppato presso la Scuola Universitaria della Svizzera Italiana ([SUPSI](http://supsi.ch/)) quale progetto semestrale del corso di *Algoritmi e Strutture Dati (C02008)*.  

#### Specifiche
Le specifiche da rispettare sono le seguenti:

    I progetti sono svolti a gruppi di 2 studenti. Il contributo di ogni
    studente deve essere chiaramente identificato sia nel codice che
    nelle presentazioni (Studente A, Studente B).
    Ogni progetto ha una fase di analisi e una realizzativa. La fase di
    analisi comprende lo studio di documentazione, una fase esplorativa
    dove diverse soluzioni vengono abbozzate e messe a confronto, e la
    selezione delle soluzioni definitive.
    Il programma deve essere scritto in C standard ISO/IEC 9899:1999.
    Nessun estensione particolare del linguaggio è consentita.
    L’implementazione deve essere verificabile. Il programma DEVE sia
    comprimere che decomprimere in due operazioni ben distinte.
##### Esecuzione del programma
    Il programma deve essere invocabile da linea di commando con la
    seguente sintassi:  
    programma opzioni file-input file-output
    dove opzioni deve essere:
    -c
    per comprimere
    -d
    per decomprimere.
    Altre opzioni sono permesse,
    ma non devono essere necessarie per l’esecuzione del programma.
    Una volta lanciato, il programma non deve necessitare più nessuna
    interazione con l’utente.  
    Il programma verrà testato in Linux nel modo seguente:
    programma –c file.input out.compresso
    programma –d out.compresso file.decompresso
    diff file.input file.decompresso
### Autori
Il codice e la documentazione, sono stati realizzati nel periodo 2017-2018 da Cristian Spozio e [Denys Vitali](https://denv.it/).


### Algoritmo
Il progetto si basa sull'algoritmo di Huffman Adattivo (FGK - Faller-Gallager-Knuth).  
Questo algoritmo si distingue dal Huffman "statico" in quanto l'albero viene generato dinamicamente con l'arrivo di nuovi byte.  
  
Grazie alla dinamicità dell'algoritmo è possibile implementare una compressione di uno stream di dati, senza conoscerne a priori il suo totale contenuto (stream video, audio o dati).  
  
Lo scopo dell'algoritmo è (ovviamente) quello di ridurre la quantità di dati da trasmettere / salvare, sfruttando le ripetizioni di byte.  
L'algoritmo risulta essere molto efficiente nella compressione di file con molte ripetizioni (come ad esempio testo), mentre risulta essere poco performante quando i dati hanno un alta entropia.  
  
L'algoritmo si basa sulla proprietà di fratellanza. L'albero generato dovrà sempre rispettare queste due condizioni:  
  
1. Tutti i nodi dell'albero, ad eccezione della radice, devono avere un fratello.
2. I nodi possono essere elencati, da sinistra a destra e dal basso verso l'alto, in ordine di peso.  

In questo esempio mostriamo un albero creato dalla codifica della parola "foobar". 
  
![Albero di "foobar"](images/graphs/foobar.jpg)

L'albero viene creato a partire da un nodo chiamato NYT (Not-Yet-Translated, "non ancora tradotto") di peso nullo.  
Questo nodo è estremamente utile in quanto nella fase di codifica e decodifica il suo percorso (nell'esempio 1000, ossia Radice -> Destra -> Sinistra -> Sinistra -> Sinistra) ci indicherà che i prossimi 8 bit saranno quelli di un nuovo carattere.  
  
#### Creazione di un albero
L'algoritmo comincia con un albero iniziale contenente un solo nodo, il NYT.   
  
![Albero vuoto](images/graphs/empty-tree.jpg){ width=100px } 
  
  
All'arrivo di un nuovo carattere (per esempio "a"), il compressore verifica se questo è già presente nell'albero. In caso contrario fa nascere dal NYT un sottoalbero con a sinistra il nuovo NYT ed a destra il nuovo elemento, come in figura.

![Albero di "a"](images/graphs/new-element.jpg){ width=100px } 
  
Essendo la proprietà di fratellanza rispettata, possiamo procedere all'aggiunta di elementi successivi.  
All'arrivo di un'altro carattere (per esempio "b"), il compressore riesegue il controllo di esistenza e fa nascere un nuovo sottoalbero dal NYT precedente.  

![Albero di "ab"](images/graphs/ab-tree.jpg){ width=100px } 

Anche in questo caso, prima dell'aggiornamento dei pesi non è necessario effettuare nessuno scambio in quanto la proprietà di fratellanza è rispettata.  
  
All'arrivo di un terzo carattere ("c"), la situazione cambia.  
  
![Albero di "abc"](images/graphs/abc-tree.jpg){ width=200px }  
  
In questo caso il nodo interno che in precedenza aveva peso 1 deve essere scambiato con "a" in quanto questo incrementerà di valore ed andrà a rompere la proprietà di fratellanza. La tecnica alla base dello scambio è quella di andare a scambiare il nodo al quale verrà incrementato il peso con l'ultimo del suo peso e così via per il *parent* che acquisirà.
  
![Albero corretto di "abc"](images/graphs/abc-tree-fixed.jpg){ width=150px }  
  
La proprietà di fratellanza è così rispettata, ed il compressore può quindi continuare a creare l'albero in modo dinamico.


#### Codifica

Compressore e decompressore concordano sul medoto di indirizzamento dei nodi quale l'invio del bit 1 se, nel percorso per raggiungere il nodo, si va verso il *child* di destra oppure del bit 0 nel caso di quello di sinistra. Questa scelta è effettuata puramente a livello di codice (non varia al variare del file). Durante l'aggiunta degli elementi nell'albero, il compressore prepara i dati da inviare. All'arrivo di un carattere non ancora visto viene inviato il percorso del NYT (nell'ultimo albero 100), e gli 8 bit del byte relativo al carattere. Nel caso in cui l'elemento è già stato visto, viene inviata la sua posizione nell'albero, facendo così risparmiare molti bit in caso di ripetizioni continue (ad esempio, per rappresentare "a" ci basterà inviare "0").  
  
Al termine della compressione, nel caso in cui il byte non sia completato (lavorando con i bit potrebbe capitarci di non avere un bitstream divisibile per 8), la nostra implementazione invia il percorso del NYT, ma dato che gli 8 bit successivi non esistono (perché appunto lo stream termina), il decompressore capirà che il suo lavoro è terminato.

#### Decodifica
La decodifica funziona in modo pressoché identico: all'arrivo di un nuovo byte, questo viene analizzato ed i percorsi presenti sotto forma di bit vengono verificati con l'albero del decompressore. All'arrivo del percorso del NYT, il decompressore si occupa di interpretare i prossimi 8 bit come un byte, e procede quindi all'aggiunta nel suo albero dell'elemento ed all'output su file dello stesso.  
Nel caso in cui il percorso verificato non è quello del NYT, il decompressore aggiunge l'elemento ottenuto dal percorso all'albero e manda in output il carattere associato.  
  
In questo modo, compressore e decompressore hanno sempre il medesimo albero, e possono quindi condividersi percorsi brevi per rappresentare byte frequenti.  

#### Ulteriori informazioni
Purtroppo l'algoritmo non è ben documentato, non esistono RFC o documenti che lo descrivono in modo ottimale ed in rete è presente molto poco a riguardo. Esiste però una visualizzazione grafica molto ben realizzata che vale la pena di consultare: [Visualizing Adaptive Huffman Coding, di Ben Tanen - COMP-150](http://ben-tanen.com/adaptive-huffman/).  
Con l'ausilio di una visualizzazione grafica dell'algoritmo, questo diventa di facile comprensione e può dunque essere sviluppato in maniera più efficace.  
  
### Licenza

MIT License
Copyright (c) <2017-2018> <C.Spozio, D.Vitali>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


## Suddivisione dei file  
  
La scelta che è stata fatta per la suddivisione dei file è stata quella di avere:  
- 1 file per la definizione della licenza: LICENSE  
- 1 file per la gestione dei test: minunit.h  
- 2 file per la gestione della compilazione: Makefile e defines.h  
- 3 file per la gestione della stampa e lo stile di essa: colors.h e console.h/c  
- 5 file per codifica e decodifica di Huffman Adattivo: huffman.h/c, main.c, utilities.h/c  

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
    Node* element_array[HUFFMAN_SYMBOLS];
    Node* nyt;

    char* output;
    int output_length;

    char* partial_output;
    int partial_output_length;

    int elements;
    unsigned int mode;
    unsigned char mask;
    unsigned short decoder_last_chunk;
    int decoder_byte;
    int buffer;
} HuffmanTree;
``` 
La quale identifica l'albero con i suoi attributi (*root* e *nyt*).
Contiene inoltre un *array* utilizzato per avere un accesso più veloce ai nodi dell'albero ed il rispettivo numero (*tree* ed *elements*).
Le componenti: *output*, *partial_output* ,*output_length*, *partial_output_length* sono necessarie per le stampe su file e a video.  

Il campo *mode* è necessario in quanto alcune delle funzioni ausiliarie (come per esempio `add_new_element`) hanno comportamenti differenti in caso di compressione o decompressione.  
La componente *mask* è utilizzata per scrivere i byte bit per bit.  
  
L'array `element_array` contiene i puntatori ai nodi, ordinati per carattere. È così possibile ottenere il puntatore al nodo desiderato semplicemente utilizzando la sintassi `ht->element_array[carattere]`. Questo array rende molto veloce la funzione di ricerca dei nodi (`find_nodes`) che è chiamata ad ogni chiamata di `add_new_element`. L'elemento NYT sarà sempre in ulitma posizione (256).

# Istruzioni per l'utilizzo  

## Compilazione  

*Nota: Per maggiori opzioni di compilazione, consultare il file README.md presente nella root del progetto*  

Per compilare il progetto è sufficiente utilizzare il comando:
```
make release
```
Verrà così generata la versione di release (senza debugging symbols ed ottimizzata), dal nome `viz-release`.  
Per altri target (come `debug` o `test` si consulti il README.md)  

## Esecuzione  

### Compressione  
La sintassi di compressione è la seguente:  
```
./viz-release -c input output.viz
```
dove l'argomento `-c` serve per eseguire il programma in compressione, `input` è un file di input con una qualsiasi estensione e `output.viz` è il nome del file che si vorrà avere in output.  
È inoltre possibile specificare il flag `-f` per forzare la sovrascrittura del file `output.viz` nel caso in cui questo sia già presente.  
    
**Nota:** È *consigliato* (ma non necessario) mantenere l'estensione `.viz` per riconoscere visualmente quali file sono stati compressi.  
   
**Nota 2:** La sintassi di compressione è stata **imposta** dai requisiti del progetto. Questa sintassi è opposta alla sintassi classica di *qualsiasi altro compressore* presente in UNIX. Per ripristinare il comportamento e mantenere la consistenza che UNIX necessita, si setti il flag `INVERTED_COMPRESSION_FLAG` in `defines.h` a `0`.

### Decompressione  
La sintessi per la decompressione è invece la seguente:  
```
./viz-release -d input.viz
```  
dove l'argomento `-d` serve per eseguire il programma in decompressione e `input.viz` è il file di input (generato precedentemente con il flag `-c`) da decomprimere.
<!--
<div align="center">

![Jenkins](./images/god.png)
# Y'all mind if I praise the Lord?
</div>
-->


# Performance

## Velocità

### Senza `ht->element_array`

| Nome file                                     | Dim (kB) | Dim compr. (kB) | Comp. rate | C [s] | D [s] | C [kB/s] | D [kB/s] | 
|-----------------------------------------------|-----------------|----------------|-----------------------|--------------------|----------------------|-------------------|---------------------| 
| bible.txt                                     | 4347            | 2518           | -72.67%               | 7.15               | 7.68                 | 608.0             | 566.0               | 
| adaptivehuffman.txt                           | 8               | 5              | -64.80%               | 0.02               | 0.02                 | 377.5             | 377.5               | 
| 100000.txt                                    | 575             | 256            | -124.91%              | 0.51               | 0.6                  | 1127.6            | 958.5               | 
| sinusoide.txt                                 | 6               | 4              | -52.48%               | 0.02               | 0.02                 | 304.7             | 304.7               | 
| immagine.tiff                                 | 3274            | 3170           | -3.29%                | 3.85               | 3.83                 | 850.5             | 854.9               | 
| alice.txt                                     | 164             | 96             | -70.50%               | 0.3                | 0.32                 | 545.3             | 511.2               | 
| 32k_random                                    | 32              | 32             | 0.05%                 | 0.28               | 0.12                 | 114.3             | 266.7               | 
| 32k_ff                                        | 32              | 4              | -697.47%              | 0.01               | 0.02                 | 3200.0            | 1600.0              | 
| It Is Wednesday My Dudes.mp4 | 703             | 703            | 0.01%                 | 1.2                | 0.72                 | 585.8             | 976.3               | 
| 04.bin                                        | 1024            | 1024           | 0.00%                 | 7.25               | 3.71                 | 141.2             | 276.0               | 
| denys.gif                                     | 25825           | 25646          | -0.70%                | 29.63              | 31.1                 | 871.6             | 830.4               | 

Table: Compressione e Decompressione a confronto  

### Con l'utilizzo `ht->element_array`
  
### Spiegazione
L'algoritmo risulta essere molto efficiente nel caso di file lunghi e con molte ripetizioni. Questa condizione è spesso verificata in file testuali, file bitmap oppure in sequenze di numeri.  
Sfortunatamente il rateo di compressione non è proprio soddisfacente nel caso in cui il file ha un alta entropia (ossia le sue frequenze sono pressoché uniformi). 
  
![Compressione kB/s per file](./images/chart-1.png)
  
![Decompressione in kB/s per file](./images/chart-2.png)  


## Consumo di memoria
### Compressione
Il compressore ha un consumo di memoria lineare di circa 47KiB, indipendente dalla dimensione del file.  
Nell'esempio qui sotto è stata compressa un immagine da 2MB tramite il comando `make massif_release_intense_c`.  
  
![Risultato di `make massif_release_intense_c`](./images/massif-release-intense-c.jpg)

### Decompressione
Il decompressore ha un consumo di memoria lineare, intorno ai 100KiB per qualsiasi tipo di file.  
Nell'esempio qui sotto è stata utilizzata un file compresso di 2MB. L'esempio è riproducibile su un qualisasi altro sistema con il comando `make massif_release_intense_d`.
  
![Risultato di `make massif_release_intense_d`](./images/massif-release-intense-d.jpg)  


## Code coverage
Con l'ausilio della code coverage fornita da Callgrind, abbiamo potuto monitorare quale funzione era chiamata più di frequente e quindi necessitava maggiore ottimizzazione.

### Compressione
Durante la fase di compressione la funzione chiamata più spesso risulta essere `add_new_element`. È quindi importante che questa sia molto efficiente al fine di ridurre al minimo i tempi di compressione.
  
![Risultato di `make callgrind_release_c`](./images/callgrind-release-c.svg){ height=800px }

### Decompressione
In decompressione la funzione chiamata più spesso è `decode_byte`, che a sua volta chiama `add_new_element`. Per questo motivo è molto importante che le due funzioni siano più efficienti possibili, al fine di avere tempi di decompressione minimi.

![Risultato di `make callgrind_release_d`](./images/callgrind-release-d.svg){ height=800px }

# Procedure di test e problemi noti   

## Problemi noti
### Efficienza dell'algoritmo
Come menzionato precedentemente, l'algoritmo non sembra essere molto efficiente nel comprimere qualsiasi tipo di file. È quindi sconsigliabile quale general-purpose compressor. Il compressore è però molto efficace in caso di documenti strutturati (come per esempio file SVG / HTML / JSON) oppure in documenti testuali (libri o estratti di pagine web).

### Compressione "lenta"
Nonostante il nostro algoritmo possa vantare di una velocità di compressione di circa 1208 kB/s (∼1.18 MB) [^1], questo risultato non è ottimale. Con un maggiore lavoro di code optimization è sicuro possibile sfiorare la soglia di 1.5 MB/s.

[^1]: Intel Core i7-6700HQ @ 8x 3.5GHz, 32GB RAM, x86_64 Linux 4.14.13-1, Spectre patch applied.

## Test effettuati  
### test_debug  
Testa se il software è in modalità `DEBUG`.

### test_create_huffman_tree  
Testa se la funzione `create_huffman_tree` viene eseguita correttamente.

### test_swap_ht_array  
Testa la funzione `swap_nodes` e confronta il risultato con quello aspettato.

### test_get_node_level
Testa la funzione `getNodeLevel` e confronta il risultato con quello aspettato.  

### test_simple_swap
Testa la funzione `swap_node` applicandola ad un esempio semplice e confronta il risultato con quello aspettato.  

### test_swap_nodes
Testa la funzione `swap_node` in tutte le sue funzioni e confronta il risultato con quello aspettato.  

### test_node_path
Testa la funzione `node_path` e confronta il risultato con quello aspettato.

### test_huffman_coding  
Testa se la codifica di Huffman viene eseguita correttamente

### test_huffman_coding_abracadabra
Il test si assicura che l'albero generato dalla codifica di "abracadabra" corrisponda a quello aspettato.  

### test_huffman_coding_abcbaaa
Il test si assicura che l'albero generato dalla codifica di "abcbaaa" corrisponda a quello aspettato.  

### test_huffman_coding_bookkeeper
Il test si assicura che l'albero generato dalla codifica di "bookkeeper" corrisponda a quello aspettato.  

![Albero risultante per "bookkeeper"](./images/graphs/bookkeeper.jpg)  
  

### test_huffman_coding_mississippi
Il test si assicura che l'albero generato dalla codifica di "mississippi" corrisponda a quello aspettato.  
  
![Albero risultante per "mississippi"](./images/graphs/mississippi.jpg)  
  

### test_huffman_coding_engineering
Il test si assicura che l'albero generato dalla codifica di "engineering" corrisponda a quello aspettato.  
  
![Albero risultante per "engineering"](./images/graphs/engineering.jpg)  
  

### test_huffman_coding_foobar
Il test si assicura che l'albero generato dalla codifica di "foobar" corrisponda a quello aspettato.  
  
![Albero risultante per "foobar"](./images/graphs/foobar.jpg)  
  

### test_huffman_coding_aardvark
Il test si assicura che l'albero generato dalla codifica di "aardvark" corrisponda a quello aspettato.  
  
![Albero risultante per "aardvark"](./images/graphs/aardvark.jpg)  


### test_huffman_coding_sleeplessness
Il test si assicura che l'albero generato dalla codifica di "sleeplessness" corrisponda a quello aspettato.  
  
![sleeplessness](./images/graphs/sleeplessness.jpg)  


### test_bin2byte
Testa la funzione bin2byte che converte una stringa di 0 ed 1 in un byte.

### test_bin2byte2
Estende il test di bin2byte per casi più complessi.

### test_byte2bin
Testa la funzione byte2bin che converte dei byte in una stringa di 0 ed 1.

### test_filename
Testa la funzione `get_filename(char* filepath)`.

### test_create_file
Testa la funzione di creazione di file

### test_write_to_file
Testa la funzionalità di scrittura su file

### test_read_file
Testa la lettura da file

### test_file_delete
Testa la funzionalità di eliminazione di file


____________________________________________________________________  
### main.c  
Il codice seguente ci permette, nel caso ci sia un singolo parametro all'avvio, di mostrare l'utilizzo del software:  
```c
    char debug_buffer[500];
    if (argc == 1) {
        usage();
        return 0;
    }
```
Utilizzando il flag `-v` possiamo ricevere informazioni riguardanti la versione del software:
```c
    if (strcmp(argv[1], "-v") == 0) {
        version();
        return 0;
    }
```
Utilizzando il flag `-c` entriamo in modalità compressione e andiamo a controllare se ci sono flag aggiuntive viene settata l'apposita flag. Nel caso non siano presenti 4 parametri e questo non sia dovuto per l'inserimento di una flag il software mostrerà come essere utilizzato correttamente e si bloccherà.    
```c
    if (strcmp(argv[1], "-c") == 0) {
        int flag_overwrite = 0;
        int flag_shift = 0;

        if (strncmp(argv[2], "-", 1) == 0) {
            if (strcmp(argv[2], "-f") == 0) {
                flag_overwrite = 1;
                flag_shift++;
            }
        }
        if (argc != 4 && flag_shift == 0) {
            usage();
            return 1;
        }

        debug("Compression");
    }

```


### huffmantree.c
La funzione `highest_numbered_node` ci ritorna il nodo con `node_number` maggiore (ossia quello più "in alto" nell'albero) dal peso pari al peso del nodo passato come parametro. Sono state effettuate scelte come far partire il ciclo dal risultato dell'espressione `node->node_number+1` e l'utilizzo di un array all'interno dell `HuffmanTree` per agevolare la ricerca. (`time(array_search) < time(recursive_on_tree_search)`) 
```c
Node* highest_numbered_node(HuffmanTree* ht, Node* node){
    int i;

    Node* highest = node;
    if(node == NULL){
        return NULL;
    }
    for(i=node->node_number+1; i<HUFFMAN_ARRAY_SIZE; i++){
        if(ht->tree[i] != NULL){
            if(ht->tree[i]->weight == node->weight){
                highest = ht->tree[i];
            }
        }
    }

    return highest;
}
```

```c
void swap_nodes(HuffmanTree* ht, Node* node, Node* node2){
    //Node* aux = node;
    if(node == NULL || node2 == NULL){
        // Null Pointer Exception
        return;
    }
    if(node->parent == NULL || node2->parent == NULL){
        // Not going to swap a root.
        return;
    }


    if(node == node2){
        // Not going to swap two identical nodes.
        return;
    }

    if(node2->parent == node || node->parent == node2){
        error("[swap_nodes] I can't swap a child with its parent");
        char* node_el1 = getElement(node);
        char* node_el2 = getElement(node2);

        char buffer[200];
        sprintf(buffer, "[swap_nodes] Tried to swap %s w/ %s", node_el1, node_el2);
        error(buffer);
        saveHuffmanTree(ht, "error-1.dot");
        sprintf(buffer, "VIZ encountered an unexpected error. Please try again or report the bug at %s", PROJECT_URL);
        error(buffer);
        exit(2);
    }
    //int distance = pos1-pos2;


    // Swap nodes w/ pointers (easy!)

    Node* parent1 = node->parent;
    Node* parent2 = node2->parent;

    Node* parent1_left = parent1->left;
    Node* parent2_left = parent2->left;


    if(parent1_left == node){
        // node is on the left of its parent
        parent1->left = node2;
    } else {
        // node is on the right of its parent
        parent1->right = node2;
    }

    if(parent2_left == node2){
        // node2 is on the left of its parent
        parent2->left = node;
    } else {
        // node2 is on the right of its parent
        parent2->right = node;
    }

    // Swap Array

    ht->tree[node2->node_number] = node;
    ht->tree[node->node_number] = node2;

    // Fix Node Numbers
    int nn = node->node_number;
    node->node_number = node2->node_number;
    node2->node_number = nn;

    node->parent = parent2;
    node2->parent = parent1;

    debug("[swap_nodes] End swap");
}

```
La funzione `node_positioner` è il core dell'algoritmo ed è quella che ci permette, come da nome, di posizionare il nodo nella posizione corretta all'interno dell'albero. Ricevendo un nodo in entrata va semplicemente a controllare se il nodo corrisponde a quello di `node_number` più alto restituito da `highest_numbered_node`, effettuato il controllo decide se è necessario lo scambio o semplicemente l'incremento del peso.
```c
void node_positioner(HuffmanTree* ht, Node* target){
    if(target == NULL){
        return;
    }
    Node* last = highest_numbered_node(ht, target);

    if(last != target && last != target->parent) {
        swap_nodes(ht, target, last);
    }
    if(last != target->parent || target->parent == ht->root)
        target->weight++;
}
```
```c
void huffman_coding_reset_partial_output(HuffmanTree* ht){
    int i;
    for(i=0; i<ht->partial_output_length; i++){
        ht->partial_output[i] = 0;
    }
    ht->partial_output_length = 0;
}

void huffman_coding_bitcheck(HuffmanTree* ht){
    if(ht->mask == 0x01){
        ht->mask = 0x80;

        ht->output[ht->output_length] = ht->partial_output[0];
        ht->partial_output[0] = 0;
#if DEBUG
        printf("Byte completed! 0x%02x, OL: %d\n", ht->output[ht->output_length] & 0xff, ht->output_length);
#endif
        ht->output_length++;

        ht->partial_output_length = 0;
        huffman_coding_reset_partial_output(ht);
    } else {
        ht->mask >>= 1;
    }
}

void huffman_append_partial_path(HuffmanTree* ht, unsigned short* path, int path_size){
    if(ht != NULL) {
        debug("[huffman_append_partial_path]");
        int i;
        for(i=0; i<path_size; i++){
            if(path[i]) {
                ht->partial_output[ht->partial_output_length] |= ht->mask;
            }
            huffman_coding_bitcheck(ht);
        }

    }
}
void huffman_append_partial_new_element(HuffmanTree* ht, unsigned short* nyt_path, int path_size, char element){
    if(ht != NULL) {
#if DEBUG
        debug("[huffman_append_partial_new_element]");
        char* string = path_to_string(nyt_path, path_size);
        printf("[huffman_append_partial_new_element] NYT is at path: %s\n", string);
        free(string);
#endif
    }


    int i;
    for(i = 0; i<path_size; i++) {
        if(nyt_path[i]) {
            ht->partial_output[ht->partial_output_length] |= ht->mask;
        }
        huffman_coding_bitcheck(ht);
    }

    debug("[huffman_append_partial_new_element] Printing element bin");
    unsigned short* binary = byte2bit(element);
    for(i = 0; i<8; i++){
        if(binary[i]) {
            ht->partial_output[ht->partial_output_length] |= ht->mask;
        }
        huffman_coding_bitcheck(ht);
    }
    free(binary);
}

void huffman_partial_final_conversion(HuffmanTree* ht){
    // Add some padding to fill the last byte.
    int* length = malloc(sizeof(int));
    unsigned short* path = node_path(ht->nyt, length);
    int path_byte = 0;

#if DEBUG
    char* path_string = path_to_string(path, *length);
    printf("Path string: %s, Mask: 0x%02x\n", path_string, ht->mask & 0xff);
#endif
    while (ht->mask != 0x00) {
        if (path_byte == *length) {
            path_byte = 0;
            continue;
        }

#if DEBUG
        printf("Path byte while loop (%d)\n",ht->partial_output_length);
#endif
        if (path[path_byte]) {
            ht->partial_output[ht->partial_output_length] |= ht->mask;
        }

        ht->mask >>= 0x01;
        path_byte++;
    }
#if DEBUG
    printf("ht->output_length: %d, ht->partial_output_length: %d\n", ht->output_length, ht->partial_output_length);
#endif
    ht->output[ht->output_length] = ht->partial_output[ht->partial_output_length];
    ht->output_length++;

    free(path);

    huffman_coding_reset_partial_output(ht);
    free(length);
}

void endHuffman(HuffmanTree* ht){
    huffman_partial_final_conversion(ht);
}

int is_compressor(HuffmanTree* ht){
    return ht->mode == H_MODE_COMPRESSOR;
}

HuffmanTree* add_new_element(HuffmanTree* ht, char c){
    Node* target = find_node(ht, (unsigned int) c & 0xff);

    int* length = malloc(sizeof(int));
    *length = 0;

#if DEBUG
    char debug_buffer[100];
    sprintf(debug_buffer, "[add_new_element] Character is %02x", c & 0xff);
    debug(debug_buffer);
#endif

    int* path_length = malloc(sizeof(int));
    unsigned short* path;

    if(target != NULL){
        debug("[add_new_element] AS");
        path = node_path(target, path_length);
        node_positioner(ht, target);
        if(is_compressor(ht)){
            huffman_append_partial_path(ht, path, *path_length);
        }
    } else {
        path = node_path(ht->nyt, path_length);

        if(is_compressor(ht)) {
            if (ht->elements == 0) {
                ht->output[0] = c;
                ht->output_length = 1;
            } else {
                huffman_append_partial_new_element(ht, path, *path_length, c);
            }
        }

        ht->elements++;
        debug("[add_new_element] NS");
        Node* old_nyt = ht->nyt;

        if(old_nyt == NULL){
            exit(51);
        }

        Node* new_nyt = create_nyt(old_nyt->node_number - 2);
        Node* new_char = create_node(old_nyt->node_number - 1, 1, c, NULL, NULL, old_nyt);

        old_nyt->weight++;
        old_nyt->left = new_nyt;
        old_nyt->right = new_char;
        old_nyt->element = -1;


        ht->nyt = new_nyt;

        new_nyt->parent = old_nyt;
        new_char->parent = old_nyt;

        ht->tree[new_nyt->node_number] = new_nyt;
        ht->tree[new_char->node_number] = new_char;

        target = old_nyt;

        ht->element_array[(unsigned int) c&0xff] = new_char;
        ht->element_array[NYT_ELEMENT] = ht->nyt;

#if DEBUG

        printf("ht->element_array elements:\n");
        int i=0;
        for(i=0; i<HUFFMAN_SYMBOLS; i++){
            printf("%d: ", i);
            printf("%s\n", getElement(ht->element_array[i]));
        }
        printf("\n\n");
#endif

    }

    free(path);
    free(path_length);
    free(length);

    while(target != ht->root){
        if(target == NULL || target->parent == NULL){
            return NULL;
        }
        target = target->parent;
        node_positioner(ht, target);
    }
    return ht;
}

void huffman_shift_partial_output(HuffmanTree* ht, int byte){
    if(byte == 0){
        return;
    }
    int i;

    char* new_ht_partial = calloc((size_t) ht->buffer * 8, 1);

    for(i=0; i<ht->partial_output_length-byte; i++){
        new_ht_partial[i] = ht->partial_output[byte + i];
    }

    ht->partial_output_length -= byte;
    free(ht->partial_output);
    ht->partial_output = new_ht_partial;
}

unsigned int get_bit(HuffmanTree* ht){
#if DEBUG
    printf("[get_bit] Byte: 0x%02x, Mask: 0x%02x\n",ht->partial_output[ht->decoder_byte] & 0xff, ht->mask & 0xff);
#endif

    unsigned int bit;
    bit = (unsigned int) (ht->partial_output[ht->decoder_byte] & ht->mask) != 0;

    if(ht->mask == 0x01){
        ht->mask = 0x80;
        ht->decoder_byte++;
    } else {
        ht->mask >>= 1;
    }
    return bit;
}


/* Used only in decompression mode! */
int decode_byte(HuffmanTree* ht){
    if(is_compressor(ht)){
        return 0;
    }

    if(ht->decoder_byte >= ht->partial_output_length){
        return 0;
    }

    if(ht->elements == 0) {
        ht->output[ht->decoder_byte] = ht->partial_output[ht->decoder_byte];
        add_new_element(ht, ht->output[ht->output_length]);
        ht->output_length++;
        ht->decoder_byte++;
        ht->elements++;
        ht->mask = 0x80;
        return ht->output_length;
    }

    Node* target = ht->root;
    unsigned int bit;
    unsigned char old_mask = ht->mask;

    int previous_decoder_byte = ht->decoder_byte;

    while(!is_leaf(target)){
        if(ht->partial_output_length - ht->decoder_byte <= 0){
            target = NULL;
            break;
        }
        bit = get_bit(ht);


        if(!bit){
            target = target->left;
        } else {
            target = target->right;
        }
    }

    if(target == NULL){
        if(ht->decoder_last_chunk && ht->partial_output_length - ht->decoder_byte <= 1){
#if DEBUG
            printf("TARGET is null - this is just padding\n");
            printf("This is the last byte. %d/%d\n", ht->decoder_byte, ht->partial_output_length);
            printf("Byte: 0x%02x, Mask: 0x%02x\n", ht->partial_output[ht->decoder_byte] & 0xff,
            ht->mask & 0xff);

            int* length = malloc(sizeof(int));
            unsigned short* nyt_path = node_path(ht->nyt, length);

            //saveHuffmanTree(ht, "out.dot");

            printf("NYT path: %s\n", path_to_string(nyt_path, *length));
            free(length);
            free(nyt_path);
#endif
            ht->mask = old_mask;
            ht->decoder_byte = previous_decoder_byte;
            return 0;
        }

        ht->mask = old_mask;
        ht->decoder_byte = previous_decoder_byte;
        return ht->output_length;
    }

    char element;

    if(is_nyt(target)){
        int i;
        char new_byte = 0x00;
        int byte_mask = 0x80;

        //printf("Nyt path received!\n");

        if(ht->decoder_last_chunk && ht->partial_output_length - ht->decoder_byte <= 1){
#if DEBUG
            printf("This is the last byte. %d/%d\n", ht->decoder_byte, ht->partial_output_length);
            printf("Byte: 0x%02x\n", ht->partial_output[ht->decoder_byte] & 0xff);
#endif
            ht->output_length++;
            return 0;
        }

        for(i=0; i<8; i++){
            if(ht->partial_output_length - ht->decoder_byte <= 0){
                ht->decoder_byte --;
                ht->mask = old_mask;
                return ht->output_length;
            }

            bit = get_bit(ht);

            if(bit == 2){
                //huffman_shift_partial_output(ht, 1);
                return ht->output_length;
            }

            if(bit){
                new_byte |= byte_mask;
            }
            byte_mask >>= 1;
        }
        element = new_byte;
    } else {

        //printf("Element received!\n");
        int* length = malloc(sizeof(int));
        unsigned short* target_path = node_path(target, length);

#if DEBUG
        printf("Target path: %s\n", path_to_string(target_path, *length));
        printf("Target element: 0x%02x\n", target->element & 0xff);
#endif

        free(length);
        free(target_path);

        element = (char) target->element;
    }

#if DEBUG
    printf("Element is: 0x%02x\n", element & 0xff);
#endif
    add_new_element(ht, element);
    ht->output[ht->output_length] = element;
    ht->output_length++;
    ht->elements++;

    return ht->output_length;
}

Node* find_nyt(Node *root){
    if(is_nyt(root)){
        return root;
    }
    Node *left = find_nyt(root->left);
    if(is_nyt(left)){
        return left;
    }

    Node *right = find_nyt(root->right);
    if(is_nyt(right)){
        return right;
    }

    return NULL; // NYT not found (It should *NEVER* happen)
}

int is_nyt(Node *pNode) {
    if(pNode == NULL){
        return 0;
    }
    if(pNode->weight == 0 && pNode->element == NYT_ELEMENT){
        return 1;
    }
    return 0;
}

int is_leaf(Node *pNode){
    if(pNode == NULL){
        return 0;
    }

    if(pNode->left == NULL && pNode->right == NULL){
        return 1;
    }

    return 0;
}

int is_internal_node(Node *pNode){
    if(pNode == NULL){
        return 0;
    }
    if(pNode->weight != 0 && pNode->element == -1){
        return 1;
    }
    return 0;
}

Node* find_node(HuffmanTree* ht, int c){
    return ht->element_array[c];
}

Node* create_nyt(int i) {
    Node* root = malloc(sizeof(Node));
    root->weight = 0;
    root->element = NYT_ELEMENT;
    root->node_number = i;
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL;
    return root;
}


Node* create_node(int node_number, int weight, int element, Node *left, Node *right, Node *parent){
    Node* n = malloc(sizeof(Node));
    n->node_number = node_number;
    n->weight = weight;
    n->element = element;
    n->left = left;
    n->right = right;
    n->parent = parent;
    return n;
}

HuffmanTree* create_huffman_tree(){
    HuffmanTree* ht = malloc(sizeof(HuffmanTree));
    Node* tmp_nyt = create_nyt(HUFFMAN_ARRAY_SIZE - 1);
    ht->root = tmp_nyt;
    ht->nyt = ht->root;
    ht->output = calloc(1, HUFFMAN_ARRAY_SIZE);
    ht->partial_output = calloc(1, 4096);
    ht->output_length = 0;
    ht->partial_output_length = 0;
    ht->elements = 0;
    ht->mode = H_MODE_COMPRESSOR;
    ht->mask = 0x80; // 1000 0000 (MSB)
    ht->decoder_byte = 0;
    ht->decoder_last_chunk = 0;
    ht->buffer = 0;

    int i;
    for(i = 0; i<HUFFMAN_ARRAY_SIZE; i++){
        ht->tree[i] = NULL;
        ht->partial_output[i] = 0;
    }

    for(i = 0; i<HUFFMAN_SYMBOLS; i++){
        ht->element_array[i] = NULL;
    }

    ht->tree[HUFFMAN_ARRAY_SIZE - 1] = ht->root;
    ht->element_array[NYT_ELEMENT] = ht->nyt;
    return ht;
}

void free_node(Node *node){
    if(node->left == NULL && node->right == NULL){
        free(node);
        return;
    }

    if(node->left != NULL){
        free_node(node->left);
    }

    if(node->right != NULL){
        free_node(node->right);
    }

    free(node);
}

void free_huffman(HuffmanTree *ht){
    free_node(ht->root);
    free(ht->output);
    free(ht->partial_output);
    free(ht);
}

unsigned short* recurse_node_path(Node* node, int* length, unsigned short* node_path){
    if(node == NULL || node->parent == NULL){
        return node_path;
    }

    if(node->parent->left == node){
        node_path[*length] = 0;
    } else {
        node_path[*length] = 1;
    }
    (*length)++;

    return recurse_node_path(node->parent, length, node_path);
}

unsigned short* node_path(Node* node, int* length){
    unsigned short* ret = malloc(HUFFMAN_ARRAY_SIZE * sizeof(unsigned short));
    *length = 0;
    unsigned short* node_path = recurse_node_path(node, length, ret);
    unsigned short* inverted_node_path = malloc(*length * sizeof(unsigned short));

    int i;
    for(i=0; i<*length; i++){
        inverted_node_path[i] = node_path[*length - 1 -i];
    }
    free(node_path);
    return inverted_node_path;
}
```
