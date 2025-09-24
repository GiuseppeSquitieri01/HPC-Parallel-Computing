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
#include "stack.h"

void init_stack(Stack* stack) {
    stack->capacity = 4;
    stack->elements = malloc(stack->capacity * sizeof(NodeCandidates));
    if (stack->elements == NULL) {
        perror("Stack allocation error");
        exit(EXIT_FAILURE);
    }
    stack->top = -1; 
}

void resize_stack(Stack* stack, int new_capacity) {
    NodeCandidates* new_elements = realloc(stack->elements, new_capacity * sizeof(NodeCandidates));
    if (new_elements == NULL) {
        perror("Error during stack resizing");
        exit(EXIT_FAILURE);
    }
    stack->elements = new_elements;
    stack->capacity = new_capacity;
}

void shrink_stack(Stack* stack) {
    if (stack->top < stack->capacity / 4 && stack->capacity > 4) {
        resize_stack(stack, stack->capacity / 2);
    }
}

void push(Stack* stack, NodeCandidates node) {
    if (stack->top == stack->capacity - 1) {
        resize_stack(stack, stack->capacity * 2);
    }
    node.flags = malloc(node.num_candidates * sizeof(bool));
    if (node.flags == NULL) {
        perror("Allocation error for flags");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < node.num_candidates; i++) {
        node.flags[i] = false;
    }

    if (node.num_candidates > 0) {
        int* candidates_copy = malloc(node.num_candidates * sizeof(int));
        if (candidates_copy == NULL) {
            perror("Allocation error for candidates copy");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < node.num_candidates; i++) {
            candidates_copy[i] = node.candidates[i];
        }
        node.candidates = candidates_copy;
    } else {
        node.candidates = NULL;
    }

    stack->top++;
    stack->elements[stack->top] = node; 
}

NodeCandidates pop(Stack* stack) {
    if (stack->top == -1) {
        perror("Stack underflow: stack is empty");
        exit(EXIT_FAILURE);
    }

    NodeCandidates node = stack->elements[stack->top];

    free(node.candidates);
    free(node.flags);

    stack->top--;

    shrink_stack(stack);

    return node; 
}

void free_stack(Stack* stack) {
    for (int i = 0; i <= stack->top; i++) {
        if (stack->elements[i].candidates != NULL) {
            free(stack->elements[i].candidates);
            stack->elements[i].candidates = NULL;
        }
        if (stack->elements[i].flags != NULL) {
            free(stack->elements[i].flags);
            stack->elements[i].flags = NULL;
        }
    }
    free(stack->elements);
    stack->elements = NULL;
    stack->top = -1;
    stack->capacity = 0;
}

NodeCandidates peek(Stack* stack) {
    if (stack->top == -1) {
        perror("Stack underflow: stack is empty");
        exit(EXIT_FAILURE);
    }

    return stack->elements[stack->top]; 
}
