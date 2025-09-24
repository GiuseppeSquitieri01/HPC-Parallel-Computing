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
#ifndef NODE_ORDERING_H
#define NODE_ORDERING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

typedef struct {
    int   node_index;    
    Node* node;          
} Context;

typedef struct {
    int* nodes;          
    int  size;           
    int  capacity;       
} Level;

typedef struct {
    Level* current_level;    
    Level* next_level;       
    bool*  visited;          
} BFSIterator;

void matching_order(Graph* G1, int* node_order);

int compare_context(const void* a, const void* b);

BFSIterator* bfs_init(Graph* g, int start);

Level* bfs_next(BFSIterator* iter, Graph* g);

void bfs_free(BFSIterator* iter);

#endif
