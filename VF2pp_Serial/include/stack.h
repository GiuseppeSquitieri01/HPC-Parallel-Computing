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
#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int   node;            
    int*  candidates;      
    bool* flags;           
    int   num_candidates;  
} NodeCandidates;

typedef struct {
    NodeCandidates* elements;  
    int             top;       
    int             capacity;  
} Stack;

void init_stack(Stack* stack);

void resize_stack(Stack* stack, int new_capacity);

void shrink_stack(Stack* stack);

void push(Stack* stack, NodeCandidates node);

NodeCandidates pop(Stack* stack);

NodeCandidates peek(Stack* stack);

void free_stack(Stack* stack);

#endif