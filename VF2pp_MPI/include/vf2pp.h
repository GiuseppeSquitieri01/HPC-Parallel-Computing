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
#ifndef VF2PP_H
#define VF2PP_H

#include "graph.h"
#include <stdbool.h>
typedef struct {
    bool result;   
    double total_wait_time;   
} VF2Result;

bool vf2pp_is_isomorphic(Graph* G1, Graph* G2);

void _restore_Tinout(Graph* G2, int popped_node2, bool* T2_tilde);
void _update_Tinout(Graph* G2, int new_node2, bool* T2_tilde);

#ifdef MPI_VERSION
VF2Result vf2pp_is_isomorphic_mpi(Graph* G1, Graph* G2, int rank, int size); 
#endif

#endif