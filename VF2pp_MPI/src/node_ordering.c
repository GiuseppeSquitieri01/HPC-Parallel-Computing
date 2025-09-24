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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node_ordering.h"

int compare_context(const void* a, const void* b) {
    Context* context_a = (Context*)a;
    Context* context_b = (Context*)b;
    return context_b->node->num_neighbors - context_a->node->num_neighbors;
}

BFSIterator* bfs_init(Graph* g, int start) {
    BFSIterator* iter = (BFSIterator*)malloc(sizeof(BFSIterator));
    iter->visited = (bool*)malloc(g->num_nodes * sizeof(bool));
    memset(iter->visited, 0, g->num_nodes * sizeof(bool));

    iter->current_level = (Level*)malloc(sizeof(Level));
    iter->next_level = (Level*)malloc(sizeof(Level));

    iter->current_level->capacity = 0;
    iter->next_level->capacity = 0;
    iter->current_level->nodes = NULL;
    iter->next_level->nodes = NULL;

    iter->current_level->size = 0;
    iter->next_level->size = 0;

    iter->visited[start] = 1;

    iter->current_level->capacity = 1;
    iter->current_level->nodes = (int*)malloc(iter->current_level->capacity * sizeof(int));
    iter->current_level->nodes[iter->current_level->size++] = start;

    return iter;
}

Level* bfs_next(BFSIterator* iter, Graph* g) {
    if (iter->current_level->size == 0) {
        return NULL;  
    }

    iter->next_level->size = 0;

    int max_next_size = 0;
    for (int i = 0; i < iter->current_level->size; i++) {
        int node = iter->current_level->nodes[i];
        max_next_size += g->nodes[node].num_neighbors;
    }

    iter->next_level->capacity = max_next_size;
    iter->next_level->nodes = (int*)malloc(iter->next_level->capacity * sizeof(int));

    for (int i = 0; i < iter->current_level->size; i++) {
        int node = iter->current_level->nodes[i];
        for (int j = 0; j <g->nodes[node].num_neighbors; j++) {
            int nbr = g->nodes[node].neighborhood[j];
            if (!iter->visited[nbr]) {
                iter->visited[nbr] = true;
                iter->next_level->nodes[iter->next_level->size++] = nbr;
            }
        }
    }

    Level* result = iter->current_level;
    iter->current_level = iter->next_level;
    iter->next_level = result;

    return iter->current_level;
}

void bfs_free(BFSIterator* iter) {
    free(iter->visited);
    free(iter->current_level->nodes);
    free(iter->next_level->nodes);
    free(iter->current_level);
    free(iter->next_level);
    free(iter);
}

void matching_order(Graph* G1, int* node_order) {
    int size = G1->num_nodes;
    bool* ordered = (bool*)malloc(size*sizeof(bool));
    memset(ordered, 0, size * sizeof(bool));

    int num_nodes_ordered = 0;
    while (num_nodes_ordered < size) {
        int max_neighbors = -1;  
        int max_node = -1;  
        
        for (int i = 0; i < G1->num_nodes; i++) {
          if (!ordered[i] && G1->nodes[i].num_neighbors > max_neighbors) {
              max_neighbors = G1->nodes[i].num_neighbors;
              max_node = i;  
          }
        }
        
        if (max_node == -1) {
            break;
        }
        node_order[num_nodes_ordered++] = max_node;
        ordered[max_node] = true;

        BFSIterator* iter = bfs_init(G1, max_node);
        Level* level;

        while ((level = bfs_next(iter, G1)) != NULL) {
            Context* context_array = malloc(level->size * sizeof(Context));
            if (context_array == NULL) {
                perror("Memoria insufficiente");
                break;
            }

            int valid_nodes = 0;
            for (int i = 0; i < level->size; i++) {
                int node_index = level->nodes[i];
                if (!ordered[node_index]) {
                    context_array[valid_nodes].node_index = node_index;
                    context_array[valid_nodes].node = &G1->nodes[node_index];
                    valid_nodes++;
                }
            }

            qsort(context_array, valid_nodes, sizeof(Context), compare_context);

            for (int i = 0; i < valid_nodes; i++) {
                if (num_nodes_ordered < size) {
                    node_order[num_nodes_ordered++] = context_array[i].node_index;
                    ordered[context_array[i].node_index] = true;
                }
            }

            free(context_array);
            context_array = NULL; 
        }

        bfs_free(iter);
    }
    free(ordered);
}
