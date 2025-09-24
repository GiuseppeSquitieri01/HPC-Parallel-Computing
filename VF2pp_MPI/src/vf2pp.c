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
#include <mpi.h>
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

#define TAG_REQUEST_WORK 100
#define TAG_WORK_DATA 101
#define TAG_NO_WORK 102
#define TAG_FOUND_ISO 103
#define TAG_FINISHED 104

#define MAX_NODES 10000
#define MAX_CANDIDATES 1000


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







VF2Result vf2pp_is_isomorphic_mpi(Graph* G1, Graph* G2, int rank, int size) {

    double total_wait_time = 0;
    double start_time,end_time;
    if (G1->num_nodes == 0 || G2->num_nodes == 0) {
        VF2Result ret;
        ret.result = false;
        return ret;
    }
    if (G1->num_nodes != G2->num_nodes) {
        VF2Result ret;
        ret.result = false;
        return ret;
    }

    bool* T2_tilde = (bool*)malloc(G2->num_nodes * sizeof(bool));
    memset(T2_tilde, 1, G2->num_nodes * sizeof(bool));
    
    int* node_order = (int*)malloc(G1->num_nodes * sizeof(int));
    matching_order(G1, node_order);
    
    int* all_candidates = NULL;
    int total_candidates = 0;
    
    if (rank == 0) {
        all_candidates = _find_candidates(node_order[0], G1, G2, &total_candidates, T2_tilde);
    }
    int same_degree_count = 0;
    for(int i = 0; i < G2->num_nodes; i++) {
        if(G2->nodes[i].num_neighbors == G1->nodes[node_order[0]].num_neighbors) {
            same_degree_count++;
        }
    }
    
    start_time = MPI_Wtime();
    MPI_Bcast(&total_candidates, 1, MPI_INT, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    total_wait_time += end_time - start_time;

    start_time=0;
    end_time=0;
    if (total_candidates == 0) {
        if (all_candidates) free(all_candidates);
        free(T2_tilde);
        free(node_order);
        VF2Result ret;
        ret.result = false;
        return ret;
    }
    
    if (rank != 0) {
        all_candidates = (int*)malloc(total_candidates * sizeof(int));
    }
    start_time = MPI_Wtime();
    MPI_Bcast(all_candidates, total_candidates, MPI_INT, 0, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    total_wait_time += end_time - start_time;
    
    start_time=0;
    end_time=0;
    int candidates_per_process = total_candidates / size;
    int remainder = total_candidates % size;
    (void)remainder; // Suppress unused variable warning
    
    int start_idx, end_idx;
    if (rank < size - 1) {
        start_idx = rank * candidates_per_process;
        end_idx = start_idx + candidates_per_process;
    } else {
        start_idx = rank * candidates_per_process;
        end_idx = total_candidates;
    }
    
    int my_candidates_count = end_idx - start_idx;
    
    int* my_candidates = (int*)malloc(my_candidates_count * sizeof(int));
    for (int i = 0; i < my_candidates_count; i++) {
        my_candidates[i] = all_candidates[start_idx + i];
    }
    
    Stack stack;
    init_stack(&stack);
    
    int matching_node = 1;
    int num_mapping = 0;
    bool found_isomorphism = false;
    bool should_terminate = false;
    
        if (my_candidates_count > 0) {
            
            NodeCandidates node;
            node.node = node_order[0];
            node.candidates = my_candidates;
            node.num_candidates = my_candidates_count;
           
            push(&stack, node);
            
            while (stack.top != -1 && !found_isomorphism && !should_terminate) {
                bool found = false;
                NodeCandidates* node_candidate_current = &stack.elements[stack.top];
                int current_node = node_candidate_current->node;
                int candidates_nodes_size = node_candidate_current->num_candidates;
                
           
                for (int i = 0; i < candidates_nodes_size && !found_isomorphism; i++) {
                    int candidate = node_candidate_current->candidates[i];
                    
                    if (node_candidate_current->flags[i] == false) {
                        
                        if (num_mapping == G2->num_nodes - 1) {
                            G1->nodes[current_node].mapped = candidate;
                            found_isomorphism = true;
                            break;
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
                
                if (!found) {
                    pop(&stack);
                    matching_node--;
                    
                    if (stack.top != -1) {
                        NodeCandidates* popped_node = &stack.elements[stack.top];
                        int popped_node1 = popped_node->node;
                        int popped_node2 = G1->nodes[popped_node1].mapped;
                        
                        G1->nodes[popped_node1].mapped = -1;
                        G2->nodes[popped_node2].mapped = -1;
                        num_mapping--;
                        
                        _restore_Tinout(G2, popped_node2, T2_tilde);
                    }
                }

                int flag;
                MPI_Iprobe(MPI_ANY_SOURCE, TAG_FOUND_ISO, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
                if (flag) {
                    should_terminate = true;
                    break;
                }
                
              
            }
            
          
        }
        
        
        
    
    if (found_isomorphism) {
        int result = 1;

        for (int i = 0; i < size; i++) if (i != rank) {
            start_time = MPI_Wtime();
            MPI_Send(&result, 1, MPI_INT, i, TAG_FOUND_ISO, MPI_COMM_WORLD);
            end_time = MPI_Wtime();
            total_wait_time += end_time - start_time;
        }

    }
    
    int local_result = found_isomorphism ? 1 : 0;
    int global_result = 0;

    start_time = MPI_Wtime();
    MPI_Allreduce(&local_result, &global_result, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    total_wait_time += end_time - start_time;
    
    start_time=0;
    end_time=0;
 
    free(node_order);
    free(T2_tilde);
    free(all_candidates);
    if (my_candidates) free(my_candidates);
    VF2Result risultati;
    risultati.result= global_result>0;
    risultati.total_wait_time = total_wait_time;
    
    return risultati;
}

bool vf2pp_is_isomorphic(Graph* G1, Graph* G2) {
    int rank, size;
    VF2Result risultati;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        printf("VF2++ MPI Statico con %d processi\n", size);
        printf("Grafi: G1 (%d nodi), G2 (%d nodi)\n", G1->num_nodes, G2->num_nodes);
        printf("Supporto: max %d nodi, max %d candidati\n", MAX_NODES, MAX_CANDIDATES);
    }

    double max_time, min_time;
    double work;
    double start_time = MPI_Wtime();
    risultati = vf2pp_is_isomorphic_mpi(G1, G2, rank, size);
    double end_time = MPI_Wtime();
    double local_time = end_time - start_time;
    
    MPI_Reduce(&local_time, &work, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    double time_of_work_process = end_time - start_time - risultati.total_wait_time;
    MPI_Reduce(&time_of_work_process, &min_time, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&time_of_work_process, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    printf("Processo %d Tempo: %.6f secondi\n", rank, time_of_work_process);

    double *all_times = NULL;
    if (rank == 0) {
        all_times = malloc(size * sizeof(double));
    }
    MPI_Gather(&time_of_work_process, 1, MPI_DOUBLE,
               all_times, 1, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    if(rank == 0){
        printf("Risultato: %s\n", risultati.result ? "ISOMORFISMO" : "NO ISOMORFISMO");
        printf("///////////////////////////////////////////\n");
        printf("Maxtimework: %.6f\n", max_time);
        printf("Mintimework: %.6f\n", min_time);
        printf("Tempo alg: %.6f\n", work);
        printf("///////////////////////////////////////////\n");

        FILE *fp = fopen("algorithm_time", "w");
        if (fp == NULL) {
            perror("Errore apertura file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int p = 0; p < size; p++) {
            fprintf(fp, "%d:%.6f\n", p, all_times[p]);
        }

        fprintf(fp, "%d\n", risultati.result);  
        fprintf(fp, "%.6f\n", work);            

        fclose(fp);
        free(all_times);
    }

    return risultati.result;
}


