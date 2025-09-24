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
#define MAX_LINE_LENGTH 50007

#define MAX_FL_LENGTH 10
#define MAX_NEIGHBOUR_LIST 8000
#define INT_MIN 0
#define INT_MAX 15000

#include "graph.h"
#include "node_ordering.h"
#include "errno.h"
#include "limits.h"
#include "ctype.h"

Graph* createGraph(int num_nodes) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (g == NULL) {
        return NULL;
    }
    
    g->nodes = (Node*)malloc(num_nodes * sizeof(Node));
    if (g->nodes == NULL) {
        free(g);
        return NULL;
    }
    
    g->num_nodes = num_nodes;

    for (int i = 0; i < num_nodes; i++) {
        g->nodes[i].neighborhood = NULL;
        g->nodes[i].num_neighbors = 0;
        g->nodes[i].ID = i;
        g->nodes[i].mapped = -1;
    }
    
    return g;
}



/**
 * @brief Libera completamente la memoria allocata per un grafo
 * 
 * Dealloca ricorsivamente tutte le liste di adiacenza, i nodi
 * e la struttura del grafo stesso, prevenendo memory leak.
 */
void freeGraph(Graph* g) {
    if (g == NULL) {
        return; // Controllo di sicurezza per puntatore NULL
    }
    
    // Liberazione di tutte le liste di adiacenza
    for (int i = 0; i < g->num_nodes; i++) {
        free(g->nodes[i].neighborhood);
    }
    
    // Liberazione array nodi e struttura grafo
    free(g->nodes);
    free(g);
}

void addEdges(Graph* g, int node, int* list, int neighbours) {
    if (node >= g->num_nodes) {
        return;
    }

    Node* targetNode = &g->nodes[node];
    
    int* tempNodeNL = (int*)realloc(targetNode->neighborhood, neighbours * sizeof(int));
    if (tempNodeNL == NULL) {
        exit(EXIT_FAILURE);
    }
    
    targetNode->neighborhood = tempNodeNL;
    targetNode->num_neighbors = neighbours;

    for (int i = 0; i < neighbours; i++) {
        targetNode->neighborhood[i] = list[i];
    }
}

Graph* readGraphFromFile(const char *filename, bool VERBOSE) {
    (void)VERBOSE; // Suppress unused parameter warning
    if (!filename) {
        perror("ERRORE: Nome file NULL");
        return NULL;
    }
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("ERRORE: Impossibile aprire il file");
        return NULL;
    }

    char firstLine[MAX_FL_LENGTH];
    int numNodes = 0, numLines = 0;

    if (fgets(firstLine, MAX_FL_LENGTH, file)) {
        sscanf(firstLine, "%d %d", &numNodes, &numLines);
    } else {
        fclose(file);
        return NULL;
    }

    Graph* g = createGraph(numNodes);
    if (g == NULL) {
        fclose(file);
        return NULL;
    }

    char line[MAX_LINE_LENGTH];
    int nLine = 0;
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        process_line(g, line, nLine == 0);
        nLine++;
    }

    fclose(file);

    return g;
}

void process_line(Graph* g, char *line, int isFirstLine) {
    (void)isFirstLine; // Suppress unused parameter warning
    int node;
    char *remainingString;

    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    remainingString = strchr(line, '\t');
    if (remainingString == NULL) {
        return;
    }

    *remainingString = '\0';
    remainingString++;

    sscanf(line, "%d", &node);

    int *adjList = NULL;
    int neighbours = 0;
    int capacity = 100;
    
    adjList = (int*)malloc(capacity * sizeof(int));
    if (!adjList) {
        return;
    }
    
    char *token = strtok(remainingString, " ");

    while (token != NULL) {
        char *endptr;
        errno = 0;
        
        while (isspace((unsigned char)*token)) token++;
        if (*token == '\0') {
            token = strtok(NULL, " ");
            continue;
        }
        
        if (neighbours >= capacity) {
            capacity *= 2;
            int *temp = (int*)realloc(adjList, capacity * sizeof(int));
            if (!temp) {
                free(adjList);
                return;
            }
            adjList = temp;
        }
        
        long val = strtol(token, &endptr, 10);

        if (errno == ERANGE) {
        } else if (endptr == token) {
        } else if (*endptr != '\0' && !isspace((unsigned char)*endptr)) {
        } else if (val < INT_MIN || val > INT_MAX) {
        } else {
            adjList[neighbours++] = (int)val;
        }

        token = strtok(NULL, " ");
    }

    addEdges(g, node, adjList, neighbours);
    
    free(adjList);
}

void printGraph(Graph* g) {
    if (g == NULL) {
        return;
    }
}
