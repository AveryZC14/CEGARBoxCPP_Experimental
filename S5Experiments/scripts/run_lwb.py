import subprocess
import os
import shutil
import pandas as pd
import run_benchmark as rb

generator_dir = "./LWB-benchmark-generator/"
benchmark_file_dir = "../benchmarks/LWB/"

problem_sets = [
    # ---------------------------------

    "s4_45_n 2 18 2",
    "s4_45_n 20 940 20",
    "s4_45_p 2 18 2",
    "s4_45_p 20 940 20",

  # ---------------------------------

    "s4_branch_n 1 30 1",
    "s4_branch_n 32 78 2",
    "s4_branch_n 80 88 8",
    "s4_branch_p 1 30 1",
    "s4_branch_p 32 78 2",
    "s4_branch_p 80 88 8",

  # ---------------------------------

    "s4_grz_n 10 90 10",
    "s4_grz_n 100 4000 100",
    "s4_grz_n 4500 7500 500",
    "s4_grz_p 10 90 10",
    "s4_grz_p 100 4000 100",
    "s4_grz_p 4500 7500 500",

  # ---------------------------------

    "s4_ipc_n 1 56 1",
    "s4_ipc_p 1 56 1",

  # ---------------------------------

    "s4_md_n 1 56 1",
    "s4_md_p 1 56 1",

  # ---------------------------------

    "s4_path_n 2 18 2",
    "s4_path_n 20 390 10",
    "s4_path_n 400 1200 100",
    "s4_path_p 2 18 2",
    "s4_path_p 20 390 10",
    "s4_path_p 400 1200 100",

  # ---------------------------------

    "s4_ph_n 1 56 1",
    "s4_ph_p 1 56 1",

  # ---------------------------------

    "s4_s5_n 1 56 1",
    "s4_s5_p 1 56 1",

  # ---------------------------------

    "s4_t4p_n 10 90 10",
    "s4_t4p_n 100 2900 100",
    "s4_t4p_n 3000 7500 500",
    "s4_t4p_n 8000 15000 1000",
    "s4_t4p_p 10 90 10",
    "s4_t4p_p 100 2900 100",
    "s4_t4p_p 3000 7500 500",
    "s4_t4p_p 8000 15000 1000",

  # ---------------------------------

################################################################################

# KT : {kt_45, branch, dum, md, grz, path, ph, poly, t4p}
# This script will generate 1008 benchmarks for the modal logic KT.
# There will be 504 benchmarks   satisfiable in KT;
# There will be 504 benchmarks unsatisfiable in KT.

  # ---------------------------------

    "kt_45_n 2 18 2",
    "kt_45_n 20 940 20",
    "kt_45_p 2 18 2",
    "kt_45_p 20 940 20",

  # ---------------------------------

    "kt_branch_n 1 30 1",
    "kt_branch_n 32 78 2",
    "kt_branch_n 80 88 8",
    "kt_branch_p 1 30 1",
    "kt_branch_p 32 78 2",
    "kt_branch_p 80 88 8",

  # ---------------------------------

    "kt_dum_n 8 40 8",
    "kt_dum_n 80 400 40",
    "kt_dum_n 480 3760 80",
    "kt_dum_p 8 40 8",
    "kt_dum_p 80 400 40",
    "kt_dum_p 480 3760 80",

  # ---------------------------------

    "kt_md_n 1 56 1",
    "kt_md_p 1 56 1",

  # ---------------------------------

    "kt_grz_n 10 90 10",
    "kt_grz_n 100 4000 100",
    "kt_grz_n 4500 7500 500",
    "kt_grz_p 10 90 10",
    "kt_grz_p 100 4000 100",
    "kt_grz_p 4500 7500 500",

  # ---------------------------------

    "kt_path_n 2 18 2",
    "kt_path_n 20 390 10",
    "kt_path_n 400 1200 100",
    "kt_path_p 2 18 2",
    "kt_path_p 20 390 10",
    "kt_path_p 400 1200 100",

  # ---------------------------------

    "kt_ph_n 1 56 1",
    "kt_ph_p 1 56 1",

  # ---------------------------------

    "kt_poly_n 2 18 2",
    "kt_poly_n 20 940 20",
    "kt_poly_p 2 18 2",
    "kt_poly_p 20 940 20",

  # ---------------------------------

    "kt_t4p_n 10 90 10",
    "kt_t4p_n 100 2900 100",
    "kt_t4p_n 3000 7500 500",
    "kt_t4p_n 8000 15000 1000",
    "kt_t4p_p 10 90 10",
    "kt_t4p_p 100 2900 100",
    "kt_t4p_p 3000 7500 500",
    "kt_t4p_p 8000 15000 1000",

  # ---------------------------------

################################################################################

# K : {branch, d4, dum, grz, lin, path, ph, poly, t4p}
# This script will generate 1008 benchmarks for the modal logic K.
# There will be 504 benchmarks   satisfiable in K;
# There will be 504 benchmarks unsatisfiable in K.

  # ---------------------------------

    "k_branch_n 1 30 1",
    "k_branch_n 32 78 2",
    "k_branch_n 80 88 8",
    "k_branch_p 1 30 1",
    "k_branch_p 32 78 2",
    "k_branch_p 80 88 8",

  # ---------------------------------

    "k_d4_n 4 16 4",
    "k_d4_n 20 200 20",
    "k_d4_n 240 1880 40",
    "k_d4_p 4 16 4",
    "k_d4_p 20 200 20",
    "k_d4_p 240 1880 40",

  # ---------------------------------

    "k_dum_n 8 40 8",
    "k_dum_n 80 400 40",
    "k_dum_n 480 3760 80",
    "k_dum_p 8 40 8",
    "k_dum_p 80 400 40",
    "k_dum_p 480 3760 80",

  # ---------------------------------

    "k_grz_n 10 90 10",
    "k_grz_n 100 4000 100",
    "k_grz_n 4500 7500 500",
    "k_grz_p 10 90 10",
    "k_grz_p 100 4000 100",
    "k_grz_p 4500 7500 500",

  # ---------------------------------

    "k_lin_n 10 90 10",
    "k_lin_n 100 3300 100",
    "k_lin_n 3400 6000 200",
    "k_lin_p 10 90 10",
    "k_lin_p 100 900 100",
    "k_lin_p 1000 9000 1000",
    "k_lin_p 10000 200000 10000",
    "k_lin_p 225000 375000 25000",
    "k_lin_p 400000 500000 100000",

  # ---------------------------------

    "k_path_n 2 18 2",
    "k_path_n 20 390 10",
    "k_path_n 400 1200 100",
    "k_path_p 2 18 2",
    "k_path_p 20 390 10",
    "k_path_p 400 1200 100",

  # ---------------------------------

    "k_ph_n 2 18 2",
    "k_ph_n 20 250 5",
    "k_ph_p 1 56 1",

  # ---------------------------------

    "k_poly_n 2 18 2",
    "k_poly_n 20 940 20",
    "k_poly_p 2 18 2",
    "k_poly_p 20 940 20",

  # ---------------------------------

    "k_t4p_n 10 90 10",
    "k_t4p_n 100 2900 100",
    "k_t4p_n 3000 7500 500",
    "k_t4p_n 8000 15000 1000",
    "k_t4p_p 10 90 10",
    "k_t4p_p 100 2900 100",
    "k_t4p_p 3000 7500 500",
    "k_t4p_p 8000 15000 1000"
]

def make_df(commands):
    time_df = pd.DataFrame(columns=["File", "Logic Domain"]+rb.solvers)
    results_df = pd.DataFrame(columns=["File"]+rb.solvers)

    for command in commands:
        split_command = command.split()

        for i in range(int(split_command[1]), int(split_command[2])+1, int(split_command[3])):
            file_name = f"{split_command[0]}.{i:04d}.intohylo"
            domain = split_command[0].split("_")[0]

            time_df = time_df._append({"File": file_name, "Logic Domain": domain} | {solver: -1 for solver in rb.solvers}, 
                                     ignore_index=True)
            
            results_df = results_df._append({"File": file_name} | {solver: -1 for solver in rb.solvers}, ignore_index=True)
            
    time_df.set_index("File", inplace=True)
    results_df.set_index("File", inplace=True)
    return time_df, results_df

def run_lwb(timeout, curr_solvers, out_dir):
    time_df, results_df = make_df(problem_sets)
    
    for problem_set in problem_sets:
        print(f"Running SET: {problem_set}...")

        shutil.rmtree(benchmark_file_dir)
        os.makedirs(benchmark_file_dir)
        subprocess.call(f"python3 {os.path.join(generator_dir, 'generate.py')} {problem_set}", shell=True)

        file_list = os.listdir(path=benchmark_file_dir)
        file_list.sort()

        rb.run_files(file_list, benchmark_file_dir, timeout, out_dir, results_df, time_df, curr_solvers)

import sys

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--help":
        print("Usage: python3 run_lwb.py optional: <timeout> <solver1> <solver2> ... <solverN>")
        
    if len(sys.argv) > 1:
        timeout = int(sys.argv[1])
    else:
        timeout = 8

    if len(sys.argv) > 2:
        curr_solvers = sys.argv[2:]
    else:
        curr_solvers = rb.solvers

    out_dir = "../results/LWB"

    run_lwb(timeout, curr_solvers, out_dir)