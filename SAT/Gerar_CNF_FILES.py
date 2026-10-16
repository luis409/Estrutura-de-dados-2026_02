import random

def gera_cnf(n_vars, n_clauses, k=3, filename="cnf_100_100.cnf", seed=42):
    random.seed(seed)
    with open(filename, "w") as f:
        f.write(f"c {n_vars} vars, {n_clauses} clausulas, {k}-SAT aleatorio\n")
        f.write(f"p cnf {n_vars} {n_clauses}\n")
        for _ in range(n_clauses):
            vars_escolhidas = random.sample(range(1, n_vars + 1), k)
            lits = [v if random.random() < 0.5 else -v for v in vars_escolhidas]
            f.write(" ".join(map(str, lits)) + " 0\n")

# Gera 3 arquivos com dificuldades diferentes
gera_cnf(100, 100, filename="cnf_100_100_sat.cnf")      # razao 1.0 → SAT
gera_cnf(100, 426, filename="cnf_100_426_fase.cnf")     # razao 4.26 → limite de fase
gera_cnf(100, 500, filename="cnf_100_500_unsat.cnf")    # razao 5.0 → UNSAT provavel