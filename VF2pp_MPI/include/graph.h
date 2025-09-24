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
#ifndef VF2PP_GRAPH_H
#define VF2PP_GRAPH_H

#include "graph_components.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Graph* createGraph(int num_nodes);

void freeGraph(Graph* g);

void addEdge(Graph* g, int node, int* list, int neighbours);

Graph* readGraphFromFile(const char *filename, bool VERBOSE);

void process_line(Graph* g, char *line, int isFirstLine);

void printGraph(Graph* g);

#endif
