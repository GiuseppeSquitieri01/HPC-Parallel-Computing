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
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "graph.h"
#include "node_ordering.h"
#include "find_candidates.h"
#include "stack.h"
#include "vf2pp.h"

void _restore_Tinout(Graph* G2, int popped_node2, bool* T2_tilde) {
    bool is_added = false; 
    bool exit = false;
    
    for (int i = 0; i < G2->nodes[popped_node2].num_neighbors; i++) {
        int nbr = G2->nodes[popped_node2].neighborhood[i];
        
        if (G2->nodes[nbr].mapped != -1) {
            is_added = true;
        } else {
            for (int j = 0; j < G2->nodes[nbr].num_neighbors; j++) {
                int nbr2 = G2->nodes[nbr].neighborhood[j];
                if ((G2->nodes[nbr2].mapped != -1)) {
                    exit = true;
                    break;
                }
            }
            
            if (exit) {
                exit = false;
                continue;
            } else {
                T2_tilde[nbr] = false;
            }
        }
    }
    if(!is_added){
        T2_tilde[popped_node2] = true;
    }
}

void _update_Tinout(Graph* G2, int new_node2, bool* T2_tilde) {
    for(int i=0; i<G2->nodes[new_node2].num_neighbors; i++){
        int nbr = G2->nodes[new_node2].neighborhood[i];
        T2_tilde[nbr] = false;
    }
    T2_tilde[new_node2] = false;
}

bool vf2pp_is_isomorphic(Graph* G1, Graph* G2) {
    if (G1->num_nodes == 0 || G2->num_nodes == 0) {
        return false;
    }

    if (G1->num_nodes != G2->num_nodes) {
        return false;
    }

    bool* T2_tilde = (bool*)malloc(G2->num_nodes*sizeof(bool));
    if(T2_tilde == NULL){
        fprintf(stderr, "ERROR: T2 Tilde error\n");
        return false;
    }
    memset(T2_tilde, 1, G2->num_nodes * sizeof(bool));

    int* node_order = (int*)malloc(G1->num_nodes*sizeof(int));
    if(node_order == NULL){
        fprintf(stderr, "ERROR: Node order error\n");
        return false;
    }
    
    matching_order(G1, node_order);

    Stack stack;
    init_stack(&stack);

    int size_candidates;
    int* candidates = _find_candidates(node_order[0], G1, G2, &size_candidates, T2_tilde);

    NodeCandidates node;
    node.node = node_order[0];
    node.candidates = candidates;
    node.num_candidates = size_candidates;
    push(&stack, node);

    int matching_node = 1;
    int num_mapping = 0;

    while(stack.top != -1){
        bool found = false;
        NodeCandidates* node_candidate_current = &stack.elements[stack.top];
        int current_node = node_candidate_current->node;
        int candidates_nodes_size = node_candidate_current->num_candidates;
        
        for(int i=0; i< candidates_nodes_size; i++){
            int candidate = node_candidate_current->candidates[i];
            
            if(node_candidate_current->flags[i]== false){
                
                if (num_mapping == G2->num_nodes -1 ){
                    G1->nodes[current_node].mapped = candidate;
                    free_stack(&stack);
                    free(node_order);
                    free(T2_tilde);
                    return true;
                }
                
                node_candidate_current->flags[i] = true;
                G1->nodes[current_node].mapped = candidate;
                G2->nodes[candidate].mapped = current_node;
                num_mapping++;
                
                _update_Tinout(G2, candidate, T2_tilde);

                int size_next_candidates;
                int* next_candidates = _find_candidates(node_order[matching_node], G1, G2, &size_next_candidates, T2_tilde);
                
                NodeCandidates node_next;
                node_next.num_candidates = size_next_candidates;
                node_next.node = node_order[matching_node];
                node_next.candidates = next_candidates;
                push(&stack, node_next);
                matching_node++;
                found = true;
                break;
            }
        }
        if(found == false){
            pop(&stack);
            matching_node -= 1;
            
            if(stack.top != -1){
                NodeCandidates* popped_node = &stack.elements[stack.top];
                int popped_node1 = popped_node->node;
                int popped_node2 = G1->nodes[popped_node1].mapped;
                
                G1->nodes[popped_node1].mapped = -1;
                G2->nodes[popped_node2].mapped = -1;
                num_mapping -= 1;
                
                _restore_Tinout(G2, popped_node2, T2_tilde);
            }
        }
        continue;
    }
    
    free_stack(&stack);
    free(node_order);
    free(T2_tilde);
    return false;
}