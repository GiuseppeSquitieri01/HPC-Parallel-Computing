/*
 License: GNU GPLv3



 Student:
 Squitieri Giuseppe, 0622702339, g.squitieri8@studenti.unisa.it

 Assignment: Progetto VF2++ - Implementazione Seriale e Parallela MPI

 Course Lecturer: Moscato Francesco, fmoscato@unisa.it

 Assignment Requirements:
 - Progettare, realizzare e analizzare una versione parallela di VF2-pp basata su MPI
 - Confrontare i risultati con l’algoritmo originale
 - Documentazione e codice secondo le istruzioni sulla piattaforma e-learning


 Assignment Description:
 Qui trova la descrizione di un algoritmo (VF2++) per il controllo di isomorfismo tra grafi.

 https://github.com/MiviaLab/vf2lib (algoritmo originale)
 https://github.com/pfllo/VF2 (versione semplificata dell'algoritmo originale)
 https://github.com/kpetridis24/vf2-pp (vf2-++ con benchmark: il benchmark va utilizzato per confrontare i risultati con quelli dell'algoritmo originale)

 Progetti, Realizzi e Analizzi una versione parallela di VF2-pp basata su MPI
 */
#include "vf2pp.h"
#include "graph.h"
#include <mpi.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
        int VERBOSE = 0;
 
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        printf("[Rank %d] Avvio programma VF2++ parallelo\n", rank);
        fflush(stdout);
    }
    
    char* percorso_grafo1 = argv[1];   
    char* percorso_grafo2 = argv[2];   

    printf("Caricamento grafo G1 da: %s\n", percorso_grafo1);
    fflush(stdout);
    Graph* G1 = readGraphFromFile(percorso_grafo1, VERBOSE);
    if (G1 == NULL) {
        fprintf(stderr, "ERRORE: impossibile caricare il grafo G1 da %s.\n", percorso_grafo1);
        fflush(stderr);
        return EXIT_FAILURE;
    }

    printf("Caricamento grafo G2 da: %s\n", percorso_grafo2);
    fflush(stdout);
    Graph* G2 = readGraphFromFile(percorso_grafo2, VERBOSE);
    if (G2 == NULL) {
        fprintf(stderr, "ERRORE: impossibile caricare il grafo G2 da %s.\n", percorso_grafo2);
        fflush(stderr);
        freeGraph(G1);  
        return EXIT_FAILURE;
    }

    
    if (!G1 || !G2) {
        if (rank == 0) fprintf(stderr, "Errore caricamento grafi\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    

    bool isomorphic = vf2pp_is_isomorphic(G1, G2);

    if (rank == 0) {
        printf("Risultato: %s\n", isomorphic ? "ISOMORFI" : "NON ISOMORFI");
        fflush(stdout);
    }
  
   
    
    freeGraph(G1);
    freeGraph(G2);
    MPI_Finalize();
    
    return isomorphic ? 0 : 1;
}