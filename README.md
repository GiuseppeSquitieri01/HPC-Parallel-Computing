# Progetto VF2++ - Implementazione Seriale e Parallela MPI

## Informazioni sul progetto
- **Studente:** Giuseppe Squitieri
- **Titolo dell’assegnazione:** High Performance Computing – Implementazione e Ottimizzazione dell’Algoritmo VF2++ per Graph Isomorphism  
- **Anno Accademico:** 2024/2025  

---

## Descrizione
Questo progetto ha come obiettivo lo **studio e l’implementazione parallela (MPI)** dell’algoritmo **VF2++** per il controllo di **isomorfismo tra grafi**.  

La traccia fornita dal docente richiede di:  
- Analizzare il problema e l’algoritmo VF2++ (descritto nell’articolo allegato).  
- Progettare, realizzare e analizzare una versione **parallela MPI** di VF2++.  
- Confrontare le performance con le versioni già esistenti.  

Codice di riferimento fornito dal docente:  
- [vf2lib – algoritmo originale](https://github.com/MiviaLab/vf2lib)  
- [VF2 – versione semplificata](https://github.com/pfllo/VF2)  
- [vf2-pp – versione VF2++ con benchmark](https://github.com/kpetridis24/vf2-pp)  

---

## Struttura della cartella di consegna
All'interno della cartella di consegna sono presenti:

- **VF2pp_Serial/** → implementazione seriale in C dell'algoritmo VF2++.  
- **VF2pp_MPI/** → implementazione parallela MPI in C di VF2++.  
- **Makefile** → script di compilazione, pulizia ed esecuzione dei benchmark.  
- **Test_Result/** → risultati prodotti dagli script Python, organizzati per confronto, livello di ottimizzazione e numero di processi. Ogni sottocartella contiene grafici e file CSV.  
- **Data/** → dati utilizzati per i grafici della relazione (medie di più esecuzioni).  
- **Relazione.pdf** → la relazione del progetto.  

---

## Script Python
- `performance_comparison_VF2pp_Vs_VF2base.py`: confronta la versione seriale VF2++ con l'algoritmo base VF2.  
- `performance_comparison_VF2pp_VF2ppMPI.py`: confronta la versione seriale VF2++ con la versione parallela MPI.  

Entrambi gli script si trovano nella cartella principale (root) e producono i risultati nella cartella `Test_Result/`, con sottocartelle per ogni confronto, livello di ottimizzazione e numero di processi. Ogni cartella contiene grafici di confronto e file CSV con tutti i dati.


## Come usare il Makefile

Puoi eseguire l'intero workflow (compilazione + esecuzione dei test Python) con:

```
make all GRAPH_MIN=0 GRAPH_MAX=1001 GRAPH_STEP=250 MPI_PROCESSES=2,4,8
```

- `GRAPH_MIN`: dimensione minima del grafo (default: 0)
- `GRAPH_MAX`: dimensione massima del grafo (default: 501)
- `GRAPH_STEP`: incremento della dimensione (default: 100)
- `MPI_PROCESSES`: numero di processi MPI (default: 2,4)

Questo comando compila tutte le versioni degli algoritmi e lancia automaticamente i due test Python.

Se vuoi solo compilare (senza eseguire i test), puoi usare:
- `make serial` per compilare solo la versione seriale
- `make mpi` per compilare solo la versione MPI

## Target del Makefile

```
VF2++ Makefile - Target disponibili:
  all              - Compila tutte le versioni ed esegue gli script Python
  serial           - Compila solo le versioni seriali (O0-O3)
  mpi              - Compila solo le versioni MPI (O0-O3)
  python_scripts   - Esegue tutti gli script Python di benchmark
  run_pp_base      - Esegue performance_comparison_VF2pp_Vs_VF2base.py
  run_mpi_base     - Esegue performance_comparison_mpi_base.py
  run_pp_mpi       - Esegue performance_comparison_VF2pp_VF2ppMPI.py
  test_O[0-3]      - Compila solo il livello di ottimizzazione specificato
  clean            - Rimuove tutti i binari compilati
  clean_serial     - Rimuove solo i binari seriali
  clean_mpi        - Rimuove solo i binari MPI
  rebuild          - Pulisce e ricompila tutto
  rebuild_serial   - Pulisce e ricompila le versioni seriali
  rebuild_mpi      - Pulisce e ricompila le versioni MPI
  help             - Mostra questo messaggio di aiuto

Parametri personalizzabili:
  GRAPH_MIN=<num>  - Dimensione minima del grafo (default: 0)
  GRAPH_MAX=<num>  - Dimensione massima del grafo (default: 501)
  GRAPH_STEP=<num> - Incremento della dimensione (default: 100)
  MPI_PROCESSES=<lista> - Numero di processi MPI (default: 2,4)

Esempio: make all GRAPH_MIN=0 GRAPH_MAX=1001 GRAPH_STEP=250 MPI_PROCESSES=2,4,8
```

## Note aggiuntive

- La cartella `Test/` contiene i dati utilizzati per generare i grafici della relazione, ottenuti come media di più test.
- La relazione è inclusa nella consegna.
- Tutti i risultati e i grafici sono organizzati per essere facilmente inseriti nella relazione.

Per riprodurre i risultati o generarne di nuovi, usare semplicemente il Makefile come descritto sopra.

## Come testare il codice

Per testare il codice **non sono forniti grafi esterni**: i grafi di input vengono generati automaticamente dagli script Python di benchmark (`performance_comparison_VF2pp_Vs_VF2base.py` e `performance_comparison_VF2pp_VF2ppMPI.py`).  
Per eseguire i test e ottenere risultati, è sufficiente lanciare i benchmark Python come descritto sopra: gli script si occuperanno di generare i grafi necessari, eseguire i confronti e salvare i risultati nelle cartelle dedicate.

Non è necessario fornire file di grafi esterni per la valutazione delle performance.
## Licenze

- **Codice sorgente:** rilasciato sotto licenza [GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.html).  
- **Relazione (PDF):** rilasciata sotto licenza [Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International](https://creativecommons.org/licenses/by-nc-sa/4.0/).  

