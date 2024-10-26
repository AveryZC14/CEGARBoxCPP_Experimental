import random
from run_file import *

def pick_var(variables):
    if random.random() < 0.2:
        return random.choice(["true", "false"])
    else:
        return random.choice(variables)
    
def unary_random(binary_depth, unary_depth, variables):
    return random.choice(["~", "[r1]", "<r1>"]) + "(" + make_random_formula(binary_depth, unary_depth - 1, variables) + ")"

def binary_random(binary_depth, unary_depth, variables):
    lhf = make_random_formula(binary_depth - 1, unary_depth, variables)
    rhf = make_random_formula(binary_depth - 1, unary_depth, variables)

    return "(" + lhf + ") " + random.choice(["&", "|", "->", "<->"]) + " (" + rhf + ")"

def make_random_formula(binary_depth, unary_depth, variables):
    if binary_depth == 0 and unary_depth == 0:
        return " " + pick_var(variables) + " "
    
    if binary_depth == 0:
        return unary_random(binary_depth, unary_depth, variables)
    
    if unary_depth == 0:
        return binary_random(binary_depth, unary_depth, variables)
    
    if random.choice([True, False]):
        return unary_random(binary_depth, unary_depth, variables)
    else:
        return binary_random(binary_depth, unary_depth, variables)
    
def return_random_formula(binary_depth, unary_depth, variables, iter):
    for i in range(iter):
        yield make_random_formula(binary_depth, unary_depth, variables)

def compare(solver1 = "CEGAR", solver2 = "Cheetah"):
    while True:
        for num_variables in [2, 3, 4]:
            for binary_depth in [2, 3, 4]:
                for unary_depth in [2, 3, 4]:
                    for formula in return_random_formula(binary_depth, unary_depth, [f"p{i+1}" for i in range(num_variables)], 10):
                        print(f"Formula: {formula}")

                        with open("formula.tmp", "w") as f:
                            f.write("begin\n"+formula+"\nend")
                        print(f"Running {solver1}", end=" ")
                        out1 = run(solvers_cmd[solver1]("formula.tmp"), 20, solvers_out_check[solver1])
                        print(f"Running {solver2}", end=" ")
                        out2 = run(solvers_cmd[solver2]("formula.tmp"), 20, solvers_out_check[solver2])
                        print("\n\n")

                        if out1 != out2 and out1 != "TIMEOUT" and out2 != "TIMEOUT" and out2 != "ERROR":
                            print(f"Formula: {formula}\n")
                            print(f"{solver1} output: {out1}")
                            print(f"{solver2} output: {out2}")
                            print("\n\n")
                            return

import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 random_formulae.py <solver1> <solver2>")
        exit(1)

    compare(sys.argv[1], sys.argv[2])