//
// Created by luisgabrielalencar on 15/10/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <DIMACS_STANDARD.h>

#define MAX_CLAUSES 1000
#define MAX_VARS 100

#define SAT 1
#define UNSAT 0
#define UNDEFINED 2

int assignment[MAX_VARS + 1]; //GUARDA AS ATRIBUIÇÔES
int trail[MAX_VARS]; // GUARDA OS INDICES DAS ATRIBUIÇÕES
int trail_size = 0;

int mark(); // Retorna o número de vars assinadas
void undo_to(int m); // "Limpa" até a variável m de cima para baixo
void assign(int var, int val); //assina a var com val(-1 ou 1)

int literal_true(int lit);
int literal_false(int lit);
int clause_status(clause_t *c);

int unit_propagate(formule_t *F);

int dpll(formule_t *F, int(*choose_variable)(formule_t *));

int choose_variable_simple(formule_t *F);
int choose_variable_moms(formule_t *F);
int choose_variable_dlis(formule_t *F);

int mark() {
    return trail_size;
}
void undo_to(int m) {
    while (trail_size > m) {
        int var = trail[trail_size] - 1;
        assignment[var] = 0;
        trail_size--;
    }
}
void assign(int var, int val) {
    assignment[var] = val;
    trail[trail_size++] = var;
}

int literal_true(int lit) {
    int var = abs(lit);
    int val = assignment[var];
    if (val == 0) return 0; // VARIÀVEL LIVRE
    return (lit > 0 && val == 1) || (lit < 0 && val == -1);
}
int literal_false(int lit) {
    int var = abs(lit);
    int val = assignment[var];
    if (val == 0) return 0; // VARIÁVEL LIVRE
    return (lit > 0 && val == -1) || (lit < 0 && val == 1);
}
int clause_status(clause_t *c) {
    int tem_livre = 0;
    for (int var = 0; var < c->size; var++) {
        if (literal_true(c->literals[var])) {return SAT;}
        if (!literal_false(c->literals[var])) tem_livre = 1;
    }
    return tem_livre ? UNDEFINED : UNSAT;
}

int unit_propagate(formule_t *F) {
    int mudou = 1;
    int propagou = 0;
    while (mudou) {
        mudou = 0;
        for (int i = 0; i < F->num_clauses; i++) {
            int status = clause_status(F->clauses);
            if (status == SAT) continue;
            if (status == UNSAT) return -1;

            int livres = 0;
            int ultimo_livre = 0;
            clause_t *c = &F->clauses[i];

            for (int j = 0; j < c->size; j++) {
                int lit = c->literals[j];
                if (!literal_false(lit) && !literal_true(lit)) {
                    livres++;
                    ultimo_livre = lit;
                }
            }

            if (livres == 1) {
                int var = abs(ultimo_livre);
                int val = (ultimo_livre > 0)? 1: -1;
                assign(var, val);
                mudou = 1;
                propagou = 1;
            }

        }
    }
    return propagou;
}
int dpll(formule_t *F, int(*choose_variable)(formule_t *)) {
    int mark_save = mark();
    int prop = unit_propagate(F);

    if (prop == -1) {
        undo_to(mark_save);
        return UNSAT;
    }
    int todas_sat = 1;
    for (int i = 0; i < F->num_clauses; i++) {
        int status = clause_status(F->clauses);
        if (status == UNSAT) {
            todas_sat = 0;
            break;
        }
    }
    if (todas_sat == 1) {
        return SAT;
    }

    int var = choose_variable(F);
    //NENHUMA VARIÁVEL LIVRE
    if (var == 0) {
        undo_to(mark_save);
        return UNSAT;
    }
    assign(var, 1);
    if (dpll(F, choose_variable) == SAT) {return SAT;}
    undo_to(mark_save);
    assign(var, -1);
    if (dpll(F, choose_variable) == SAT) {return SAT;}
    undo_to(mark_save);
    return UNSAT;
}

int choose_variable_simple(formule_t *F) {
    for (int var = 0; var < F->num_vars; var++) {
        if (assignment[var] == 0) return var;
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
                    score += 1.0 / (1 << c->size); //QUANTO MAIOR A CLAUSULA, MENOR O SCORE
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
int choose_variable_dlis(formule_t *F) {
    int best_count = -1;
    int best_var = 0;
    for (int v = 1; v <= F->num_vars; v++) {
        if (assignment[v] != 0) continue;
        int count = 0;
        for (int i = 0; i < F->num_clauses; i++) {
            clause_t *c = &F->clauses[i];
            if (clause_status(c) != UNDEFINED) continue;
            for (int j = 0; j < c->size; j++) {
                if (abs(c->literals[j]) == v) {
                    count++;
                    break;
                }
            }
        }
        if (count > best_count) {
            best_count = count;
            best_var = v;
        }
    }
    return best_var;
}