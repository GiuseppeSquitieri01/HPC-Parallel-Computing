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
#include "graph.h"
#include "vf2pp.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
    clock_t start, end;
    double  loadGraph, VF2pp;
    
    int VERBOSE = 0;

    if (argc < 3) {
        fprintf(stderr, "ERRORE: numero di argomenti insufficiente.\n");
        fprintf(stderr, "Uso: %s <percorso_grafo1> <percorso_grafo2> <OPZIONALE:VERBOSE>\n", argv[0]);
        return EXIT_FAILURE;
    } else if (argc == 4) {
        char *endptr;
        long int val = strtol(argv[3], &endptr, 10);
        if (argv[3] != endptr && *endptr == '\0') {
            if (val == 0 || val == 1) {
                VERBOSE = (int)val;
            }
        } else {
            fprintf(stderr, "ATTENZIONE: VERBOSE deve essere 0 o 1. Impostato VERBOSE = 0.\n");
        }
    }

    start = clock();
    
    char* percorso_grafo1 = argv[1];
    char* percorso_grafo2 = argv[2];

    Graph* G1 = readGraphFromFile(percorso_grafo1, VERBOSE);
    if (G1 == NULL) {
        fprintf(stderr, "ERRORE: impossibile caricare il grafo G1 da %s.\n", percorso_grafo1);
        return EXIT_FAILURE;
    }

    Graph* G2 = readGraphFromFile(percorso_grafo2, VERBOSE);
    if (G2 == NULL) {
        fprintf(stderr, "ERRORE: impossibile caricare il grafo G2 da %s.\n", percorso_grafo2);
        freeGraph(G1);
        return EXIT_FAILURE;
    }

    if (VERBOSE) {
        printf("\nINFORMAZIONI DETTAGLIATE:\n");
        printf("Percorso grafo Token: %s\n", percorso_grafo1);
        printf("Percorso grafo Pattern: %s\n", percorso_grafo2);
        printf("Nodi G1: %d, Nodi G2: %d\n", G1->num_nodes, G2->num_nodes);
        
        printf("\nSTRUTTURA GRAFO G1:\n");
        printGraph(G1);
        printf("\nSTRUTTURA GRAFO G2:\n");
        printGraph(G2);
    }
    
    end = clock();
    loadGraph = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Grafi caricati con successo in %.4f secondi\n", loadGraph);

    printf("\nAvvio algoritmo VF2++ per ricerca isomorfismo...\n");
    start = clock();
    
    bool isomorfi = false;

    isomorfi = vf2pp_is_isomorphic(G1, G2);
    
    end = clock();
    VF2pp = ((double)(end - start)) / CLOCKS_PER_SEC;

    FILE *time_file = fopen("algorithm_time.txt", "w");
    if (time_file) {
        if(isomorfi){
            int iso_trov=1;
            fprintf(time_file, "%i\n", iso_trov);
        }
        else{
            int iso_trov=1;
            fprintf(time_file, "%i\n", iso_trov);
        }
        fprintf(time_file, "%.6f\n", VF2pp);
        fclose(time_file);
    }

    printf("\nRISULTATO ANALISI:\n");
    if (isomorfi) {
        printf("G1 and G2 are isomorph\n");
    } else {
        printf("G1 and G2 are not isomorph\n");
    }

    printf("\nPulizia memoria...\n");
    freeGraph(G1);
    freeGraph(G2);

    printf("\nSTATISTICHE PERFORMANCE:\n");
    printf("Tempo caricamento: %.4f secondi\n", loadGraph);
    printf("Tempo VF2++: %.4f secondi\n", VF2pp);
    printf("Tempo totale: %.4f secondi\n", loadGraph + VF2pp);
    
    printf("\n%.4f,%.4f\n", loadGraph, VF2pp);
    
    printf("Esecuzione completata con successo\n");
    return EXIT_SUCCESS;
}
