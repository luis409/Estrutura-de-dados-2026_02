//
// Created by luisgabrielalencar on 15/10/2026.
//

#ifndef ESTRUTURA_DE_DADOS_2026_02_DIMACS_STANDARD_H
#define ESTRUTURA_DE_DADOS_2026_02_DIMACS_STANDARD_H

#define MAX_CLAUSES 100
#define MAX_VARS 100

#define SAT 1
#define UNSAT 0
#define UNDEFINED 2

typedef struct clause {
    int literals[MAX_VARS];
    int size;
} clause_t;

typedef struct formule {
    struct clause clauses[MAX_CLAUSES];
    int num_clauses;
    int num_vars;
} formule_t;

void read_dimacs(const char *filename, formule_t *F);

#endif //ESTRUTURA_DE_DADOS_2026_02_DIMACS_STANDARD_H
