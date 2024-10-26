import subprocess
import sys
import time

MAX_VIRTUAL_MEM = 1024 * 1 # 1GB

def cegar_cmd(filename):
    print(filename)
    with open(filename, "r") as f:
        inp = f.read()

    inp = inp.split("\n")[1]
    inp = inp.replace("r1", "").replace("-", "=").replace("false", "$false").replace("true", "$true")
    
    with open("cegar_file.tmp", "w") as f:
        f.write(inp)

    return {"args": ["prlimit", f"--rss={MAX_VIRTUAL_MEM}M", "../../main", "-tb4", "-f", "cegar_file.tmp"]}

def cheetah_cmd(filename):
    with open(filename, "r") as f:
        inp = f.read()

    inp = inp.replace("p0", "p99999").replace("v", "p")

    with open("cheetah_file.tmp", "w") as f:
        f.write(inp)

    return {"args": ["prlimit", f"--rss={MAX_VIRTUAL_MEM}M", "./S5Cheetah", "cheetah_file.tmp"]}

def s52sat_cmd(filename):
    with open(filename, "r") as f:
        inp = f.read()

    inp = inp.replace("p0", "p99999").replace("v", "p")

    with open("s52sat_file.tmp", "w") as f:
        f.write(inp)

    return {"args": ["prlimit", f"--rss={MAX_VIRTUAL_MEM}M", "./S52SAT", "s52sat_file.tmp", "-diamondDegree", "-caching"]}

def lck_cmd(filename):
    with open(filename, "r") as f:
        inp = f.read()

    inp = "1:" + inp.split("\n")[1]
    inp = inp.replace("r1", "E").replace("-", "=").replace("false", "False").replace("true", "True")
    
    return {"args": ["prlimit", f"--rss={MAX_VIRTUAL_MEM}M", "./lck", "graph"], "input": inp.encode()}

def ksp_cmp(filename):
    with open(filename, "r") as f:
        file_input = f.read()

    file_input = file_input.split("\n")[1]
    file_input = file_input.replace("[r1]", " box ").replace("<r1>", " dia ")

    with open("ksp_file.tmp", "w") as f:
        f.write("sos(formulas).\n")
        f.write(file_input)
        f.write(".\nend_of_list.\n")

    cmd = ["prlimit", f"--rss={MAX_VIRTUAL_MEM}M", "../ksp-0.1.6/ksp", "-c", "../ksp-0.1.6/conf.files/cade-28/S5_euc1_euc2_ref.conf", 
           "-c", "../ksp-0.1.6/conf.files/cade-28/cord_ple_ires_K.conf", "-i", "ksp_file.tmp"]
    return {"args":cmd}

def check_out_CEGAR(process):
    if process.decode("utf-8").split("\n")[0] == "Satisfiable":
        return "SAT"
    else:
        return "UNSAT"
    
def check_out_Cheetah(process):
    if process.decode("utf-8").split("\n")[-3] == "s SATISFIABLE":
        return "SAT"
    else:
        return "UNSAT"
    
def check_out_S52SAT(process):
    if process.decode("utf-8").split("\n")[-8] == "s SATISFIABLE":
        return "SAT"
    else:
        return "UNSAT"
    
def check_out_LCK(process):
    if process.decode("utf-8").split("\n")[-3].split()[-1] == "satisfiable":
        return "SAT"
    else:
        return "UNSAT"
    
def check_out_KSP(process):
    if process.decode("utf-8").split("\n")[-2] == " Satisfiable.":
        return "SAT"
    elif process.decode("utf-8").split("\n")[-2] == " Unsatisfiable.":
        return "UNSAT"
    
    else:
        return "ERROR"
    
solvers = ["CEGAR", "Cheetah", "S52SAT", "LCK", "KSP"]

solvers_cmd = {
    "CEGAR": cegar_cmd,
    "Cheetah": cheetah_cmd,
    "S52SAT": s52sat_cmd,
    "LCK": lck_cmd,
    "KSP": ksp_cmp
}
    
solvers_out_check = {
    "CEGAR": check_out_CEGAR,
    "Cheetah": check_out_Cheetah,
    "S52SAT": check_out_S52SAT,
    "LCK": check_out_LCK,
    "KSP": check_out_KSP
}

# import resource
    
# def limit_mem(max_mem = MAX_VIRTUAL_MEM):
#     resource.setrlimit(resource.RLIMIT_AS, (max_mem, resource.RLIM_INFINITY))
    

def run(cmd, timeout, checkOutput):
    # limit_mem()
    try:
        start = time.time()
        process = subprocess.run(timeout=timeout, stdout=subprocess.PIPE, stderr=subprocess.PIPE, **cmd)
        time_taken = time.time() - start
        if process.returncode != 0:
            return "ERROR", process.stderr.decode("utf-8")
        
        ans = checkOutput(process.stdout)
        return ans, time_taken
        
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    
def print_output(out):
    if out ==  "TIMEOUT":
        print("TIMEOUT")

    elif out[0] == "ERROR":
        print("ERROR in execution: ", out[1])

    else:
        print("OK: Ans: ", out[0], " Time: ", out[1], "s")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 run_file.py <solver> <file_path>")
        exit()

    solver = sys.argv[1]
    file_path = sys.argv[2]

    cmd = solvers_cmd[solver](file_path)
    out = run(cmd, 1, solvers_out_check[solver])
    print_output(out)