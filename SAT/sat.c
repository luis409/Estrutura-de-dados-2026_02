//
// Created by luisgabrielalencar on 12/10/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAT 1
#define UNSAT 0
#define UNDEFINED 2

#define MAX_CLAUSES 1000
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

int assignment[MAX_VARS + 1];

int trail[MAX_VARS];
int trail_size = 0;

int mark(){ return trail_size; }

void undo_to(int m) {
    while (trail_size > m) {
        int var = trail[--trail_size];
        assignment[var] = 0;
    }
}
void assign(int var, int val) {
    assignment[var] = val;
    trail[trail_size++] = var;
}

int literal_true(int lit);
int literal_false(int lit);
int clause_status(clause_t *c);

int unit_propagate(formule_t *F);

//
//HEURISTICAS
//
int choose_variable_simple(formule_t *F);
int choose_variable_moms(formule_t *F);
int choose_variable_dlis(formule_t *F);
int choose_variable_vsids(formule_t *F);

int dpll(formule_t *F, int(*choose_variable)(formule_t *));

void read_dimacs(const char *filename, formule_t *F);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo.cnf>\n", argv[0]);
        return 1;
    }

    formule_t F;
    memset(&F, 0, sizeof(F));
    read_dimacs(argv[1], &F);

    int assign[MAX_VARS + 1] = {0};

    if (dpll(&F, choose_variable_moms) == SAT) {
        printf("SAT\n");
        for (int v = 1; v <= F.num_vars; v++) {
            printf("x%d = %s\n", v, assign[v] == 1 ? "true" : "false");
        }
    } else {
        printf("UNSAT\n");
    }
    return 0;
}

int literal_true(int lit) {
    int var = abs(lit);
    int val = assignment[var];
    if (val == 0) return 0;
    return (lit > 0 && val == 1) || (lit < 0 && val == -1);
}
int literal_false(int lit) {
    int var = abs(lit);
    int val = assignment[var];
    if (val == 0) return 0;
    return (lit < 0 && val == 1) || (lit > 0 && val == -1);
}
int clause_status(clause_t *c) {
    int tem_livre = 0;
    for (int i = 0; i < c->size; i++) {
        if (literal_true(c->literals[i])) return SAT;
        if (!literal_false(c->literals[i])) tem_livre = 1;
    }
    return tem_livre? UNDEFINED : UNSAT;
}

int unit_propagate(formule_t *F) {
    int mudou = 1;
    int propagou = 0;

    while (mudou) {
        mudou = 0;
        for (int i = 0; i < F->num_clauses; i++) {
            clause_t *c = &F->clauses[i];
            int status = clause_status(c);

            if (status == UNSAT) return -1;
            if (status == SAT) continue;

            int livres = 0;
            int ultimo_livre = 0;
            for (int j = 0; j < c->size; j++) {
                int lit = c->literals[j];
                if (!literal_true(lit) && !literal_false(lit)) {
                    livres++;
                    ultimo_livre  = lit;
                }
            }

            if (livres == 1) {
                int var = abs(ultimo_livre);
                int val = (ultimo_livre > 0)? 1 : -1;

                if (assignment[var] != 0 && assignment[var] != val) return -1;
                assignment[var] = val;
                mudou = 1;
                propagou = 1;
            }
        }
    }
    return propagou;
}

int choose_variable_simple(formule_t *F) {
    for (int v = 1; v <= F->num_clauses; v++) {
        if (assignment[v] == 0) return 0;
    }
    return 0;
}
int choose_variable_moms(formule_t *F) {
    double best_score = -1.0;
    int best_var = 0;

    for (int v = 1; v <= F->num_vars; v++) {
        if (assignment[v] != 0) continue;

        double score = 0.0;
        for (int i = 0; i < F->num_clauses; i++) {
            clause_t *c = &F->clauses[i];
            if (clause_status(c) != UNDEFINED) continue;

            for (int j = 0; j < c->size; j++) {
                if (abs(c->literals[j]) == v) {
                    score += 1.0 / (1 << c->size);
                    break;
                }
            }
        }

        if (score > best_score) {
            best_score = score;
            best_var = v;
        }
    }
    return best_var;
}

//int choose_variable_dlis(formule_t *F);
//int choose_variable_vsids(formule_t *F);

int dpll(formule_t *F, int(*choose_variable)(formule_t *)) {
    int mark_save = mark();

    int prop = unit_propagate(F);
    if (prop == -1) {
        undo_to(mark_save);
        return UNSAT;
    }

    int todas_sat = 1;
    for (int i = 0; i < F->num_clauses; i++) {
        if (clause_status(&F->clauses[i]) != SAT) {
            todas_sat = 0;
            break;
        }
    }
    if (todas_sat) { return SAT; }

    int var = choose_variable(F);
    if (var == 0) return UNSAT;

    assign(var, 1);
    assignment[var] = 1;
    if (dpll(F, choose_variable) == SAT) return SAT;
    undo_to(mark_save);

    assign(var, -1);
    if (dpll(F, choose_variable) == SAT) return SAT;
    undo_to(mark_save);

    return UNSAT;
}

void read_dimacs(const char *filename, formule_t *F) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir %s\n", filename);
        exit(1);
    }
    char buffer[256];
    int idx = 0;
    F->num_vars = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (buffer[0] == 'c') continue;
        if (buffer[0] == 'p') {
            sscanf(buffer, "%d %d", &F->num_vars, &F->num_clauses);
            continue;
        }
        clause_t c;
        c.size = 0;
        char *token = strtok(buffer, " \t\n");
        while (token != NULL) {
            int lit = atoi(token);
            if (lit == 0) break;
            c.literals[c.size++] = lit;
            token = strtok(NULL, " \t\n");
        }
        if (c.size > 0) F->clauses[idx++] = c;
    }
    F->num_clauses = idx;
    fclose(file);
}