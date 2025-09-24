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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "find_candidates.h"

int* _find_candidates(int u, Graph* G1, Graph* G2, int* size_candidates, bool* T2_tilde){
    Node* covered_neighbords = (Node*) malloc(G1->nodes[u].num_neighbors * sizeof(Node));
    int num_nodes = 0;

    for(int i=0; i<G1->nodes[u].num_neighbors; i++){
        int nbr = G1->nodes[u].neighborhood[i];           
        int mapping = G1->nodes[nbr].mapped;              
        if(mapping != -1){
            covered_neighbords[num_nodes++] = G2->nodes[mapping];  
        }
    }

    if(num_nodes == 0){
        int* candidates = (int*)malloc(G2->num_nodes * sizeof(int));
            int num_candidates = 0;  

        for(int i=0; i<G2->num_nodes; i++){
            if((G2->nodes[i].mapped == -1) && (T2_tilde[i]==true) && (G2->nodes[i].num_neighbors == G1->nodes[u].num_neighbors))
                candidates[num_candidates++] = i;
        }
        *size_candidates = num_candidates;
        for (int i = 0; i < G1->nodes[u].num_neighbors; i++) {
            covered_neighbords[i].neighborhood = NULL;
        }
        free(covered_neighbords);
        return candidates;
    }

    else{

        if(num_nodes  == 1){
            int* candidates = (int*)malloc(covered_neighbords[0].num_neighbors* sizeof(int));
            int num_candidates = 0;
            *size_candidates = num_candidates;
            int node = G1->nodes[covered_neighbords[0].mapped].mapped;  
            for(int i =0; i<covered_neighbords[0].num_neighbors; i++ ){
                int candidate = G2->nodes[node].neighborhood[i];
                if(((G2->nodes[candidate].num_neighbors == G1->nodes[u].num_neighbors) && (G2->nodes[candidate].mapped == -1))){
                    candidates[num_candidates++] = candidate;
                }
            }
            *size_candidates = num_candidates;
            for (int i = 0; i < G1->nodes[u].num_neighbors; i++) {
                covered_neighbords[i].neighborhood = NULL;
            }
            free(covered_neighbords);
            return candidates;
        }
        else{
            int max_dim=0;
            for(int c=0; c<num_nodes; c++){
              max_dim += covered_neighbords[c].num_neighbors;
            }
            int* candidates = (int*)malloc( max_dim* sizeof(int));
            int num_candidates = 0;
            *size_candidates = num_candidates;
            
            qsort(covered_neighbords, num_nodes, sizeof(Node), comparator);
            bool found = false; int count = 0;
            int size_covered_neighbor_0 = covered_neighbords[0].num_neighbors;
            
            for(int elem = 0; elem< size_covered_neighbor_0; elem++){
                count=0;
                int node = covered_neighbords[0].neighborhood[elem];
                if(!((G2->nodes[node].num_neighbors == G1->nodes[u].num_neighbors) && (G2->nodes[node].mapped == -1))){
                    continue;  
                }

                for(int i=1; i < num_nodes; i++){
                    for(int j=0; j<covered_neighbords[i].num_neighbors; j++){
                        if(node == covered_neighbords[i].neighborhood[j]){
                            found = true;
                            count++;
                            break;
                        }
                    }
                    if(found){
                        found = false;
                        continue;
                    }else{
                        break;  
                    }
                }
                if (count == num_nodes -1 ){
                    candidates[num_candidates++] = node;
                    *size_candidates = num_candidates;
                }
            }
            for (int i = 0; i < G1->nodes[u].num_neighbors; i++) {
                covered_neighbords[i].neighborhood = NULL;
            }
            free(covered_neighbords);
            return candidates;
        }

    }

}

int comparator(const void* a, const void* b) {
    const Node* node1 = *(const Node**)a;
    const Node* node2 = *(const Node**)b;

    return node1->num_neighbors - node2->num_neighbors;
}
