//
// Created by luisgabrielalencar on 12/10/2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int assignment[MAX_VARS + 1]; //GUARDA AS ATRIBUIÇÔES
int trail[MAX_VARS]; // GUARDA OS INDICES DAS ATRIBUIÇÕES
int trail_size = 0;

int mark();
void undo_to(int m);
void assign(int var, int val);

int literal_true(int lit); //RETORNA 1 PARA TRUE E 0 PARA FALSE OU LIVRE
int literal_false(int lit);//
int clause_status(clause_t *c);

int unit_propagate(formule_t *F);

//
//HEURISTICAS
//
int choose_variable_simple(formule_t *F);
int choose_variable_moms(formule_t *F);
int choose_variable_dlis(formule_t *F);

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


    if (dpll(&F, choose_variable_moms) == SAT) {
        printf("SAT\n");
        for (int v = 1; v <= F.num_vars; v++) {
            printf("x%d = %s\n", v, assignment[v] == 1? "true" : "false");
        }
    } else {
        printf("UNSAT\n");
    }
    return 0;
}

//RETORNA O TAMANHO DA TRILHA
int mark() {
    return trail_size;
}
//LIMPA, DE CIMA PARA BAIXO ATÉ "m"
void undo_to(int m) {
    while (trail_size > m) {
        int var = trail[--trail_size];
        assignment[var] = 0;
    }
}
//ASSINA "var" COM "val (1 OU -1)"
void assign(int var, int val) {
    assignment[var] = val;
    trail[trail_size++] = var;
}

//RETORNA 1 PARA TRUE E 0 PARA FALSE OU LIVRE
//LIVRE: LITERAL QUE AINDA NÃO FOI ASSINADO
int literal_true(int lit) {
    int var = abs(lit);
    int val = assignment[var];
    if (val == 0) return 0; // VARIÀVEL LIVRE
    return (lit > 0 && val == 1) || (lit < 0 && val == -1);
}
//RETORNA 1 PARA FLASE E 0 PARA TRUE OU LIVRE
int literal_false(int lit) {
    int var = abs(lit);
    int val = assignment[var];
    if (val == 0) return 0; // VARIÀVEL LIVRE
    return (lit < 0 && val == 1) || (lit > 0 && val == -1);
}
//TRUE,FALSE OU LIVRE
int clause_status(clause_t *c) {
    int tem_livre = 0;
    for (int i = 0; i < c->size; i++) {
        if (literal_true(c->literals[i])) return SAT;
        //
        // OU SEJA, A DADA "CLAUSULA" ESTA SATISFEITA, EX:
        //(X1 OR X2 OR 3 OR .. OR Xn)
        //APENAS UM TRUE JÁ É VÁLIDO, PARA A "CLAUSULA"
        //
        if (!literal_false(c->literals[i])) tem_livre = 1;
    }
    return tem_livre? UNDEFINED : UNSAT;
}
//RETORNA 0 SE NÃO HÁ PROPAGAÇÂO, RETORNA -1 SE HOUVE CONFLITO NA PROPAGAÇÂO
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
            int ultimo_livre = 0; //IMPORTANTE VARA SALVAR EM CASO DE UNITÁRIA
            for (int j = 0; j < c->size; j++) {
                int lit = c->literals[j];
                if (!literal_true(lit) && !literal_false(lit)) {
                    livres++;
                    ultimo_livre  = lit;
                }
            }
            // VARIÁVEL UNITÁRIA
            if (livres == 1) {
                int var = abs(ultimo_livre);
                int val = (ultimo_livre > 0)? 1 : -1;
                assign(var, val);
                mudou = 1;
                propagou = 1;
            }
        }
    }
    return propagou;
}
int dpll(formule_t *F, int(*choose_variable)(formule_t *)) {
    //SALVO O INDICE ATUAL DE tail
    int mark_save = mark();

    int prop = unit_propagate(F);
    //SE HÁ CONFLITO
    if (prop == -1) {
        undo_to(mark_save);
        return UNSAT;
    }
    //TODAS AS CLAUSULAS SÃO SAT?
    int todas_sat = 1;
    for (int i = 0; i < F->num_clauses; i++) {
        if (clause_status(&F->clauses[i]) != SAT) {
            todas_sat = 0;
            break;
        }
    }
    if (todas_sat) { return SAT; }

    //CHOOSE VAR DE ACORDO COM A HEURÍSTICA PASSADA
    int var = choose_variable(F);
    //NENHUMA VARIÁVEL LIVRE
    if (var == 0) {
        undo_to(mark_save);
        return UNSAT;
    }

    //*
    //O unit_propagate atribui várias variáveis.
    //Se ele usa assignment[var] = val direto,
    //o dpll não sabe quais variáveis desfazer.
    //*//
    assign(var, 1);
    if (dpll(F, choose_variable) == SAT) return SAT;
    undo_to(mark_save);
    assign(var, -1);
    if (dpll(F, choose_variable) == SAT) return SAT;
    undo_to(mark_save);

    return UNSAT;
}
//HEURISTICAS

//PRIMEIRA VARIÁVEL LIVRE
int choose_variable_simple(formule_t *F) {
    for (int v = 1; v <= F->num_vars; v++) {
        if (assignment[v] == 0) return v;
    }
    //TODAS AS VARIÁVEIS FORAM DEFINIDAS
    return 0;
}
//VARIÁVEL EM CLAUSULAS CURTAS
//Maximum Occurrences in clauses of Minimum Size(MOMS)
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
//FOCO EM OCORRENCIAS TOTAIS
//Dynamic Largest Individual Sum
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

void read_dimacs(const char *filename, formule_t *F) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir %s\n", filename);
        exit(1);
    }
    char buffer[256];
    int idx = 0;
    F->num_vars = 0;
    printf("\tDisplay input:\n");
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
        if (buffer[0] == 'c') continue;
        if (buffer[0] == 'p') {
            sscanf(buffer, "p cnf %d %d", &F->num_vars, &F->num_clauses);
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
    printf("\tLidas %d clausulas, num_vars = %d\n", idx, F->num_vars);
    fclose(file);
}