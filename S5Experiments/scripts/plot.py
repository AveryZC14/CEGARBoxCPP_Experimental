import sys
import pickle
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker
from run_file import solvers

solvers = ["CEGAR", "Cheetah", "S52SAT", "LCK", "KSP"]
all_benchmarks = ["QS5", "3CNF", "MQBF"]

def plot(timeouts, counts, total, name, curr_solvers = solvers):
    fig, ax = plt.subplots()
    colours = ["#B931FC", "#5CD2E6", "#FF9800", "#A22C29", "#56E39F"]

    for i, solver in enumerate(curr_solvers):
        ax.plot(timeouts, counts[solver], label=solver, c = colours[i])
    # ax.plot(timeouts, [x[0] for x in counts], label="CEGAR", c = "#B931FC")
    # ax.plot(timeouts, [x[1] for x in counts], label="Cheetah", c = "#5CD2E6")

    ax.plot(timeouts, [total for x in timeouts], label="Total", c = "#FF0000", linestyle="--")
    ax.set_xlabel("Given Time (in s, log scale)")

    ax.set_xscale("log", base=2)
    ax.set_xticks(timeouts)
    ax.get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())

    ax.set_ylabel("Number of instances solved")
    ax.legend()
    ax.grid(linestyle="--", color="grey", linewidth=0.5)

    plt.savefig(f"../plots/{'_'.join(name)}.png")
    plt.show()

def make_counts(times_df, timeouts, curr_solvers):
    counts = {solver: [] for solver in curr_solvers}
    
    print(f"Making counts for {len(times_df)} instances: {timeouts}")
    for solver in solvers:
        for timeout in timeouts:
            counts[solver].append(((times_df[solver] <= timeout) & (times_df[solver] != -1)).sum())

    return counts

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 plot.py <benchmark_name or ALL for all benchmarks> optional: <timeout>")
        exit()

    benchmarks = [sys.argv[1]]
    if benchmarks == ["ALL"]:
        benchmarks = all_benchmarks
    print(benchmarks)
    if len(sys.argv) > 2:
        timeout = int(sys.argv[2])
    else:
        timeout = 8

    timeouts = [timeout/(2**i) for i in range(5, -1, -1)]
    print(timeouts)
    
    if len(sys.argv) > 3 and sys.argv[3] != "ALL":
        curr_solvers = sys.argv[3:]
    else:
        curr_solvers = solvers

    counts = None
    total = 0

    for bench in benchmarks:
        times_df = pd.read_csv(f"../results/{bench}/times.csv")
        c = make_counts(times_df, timeouts, solvers)
        t = len(times_df)
        
        total += t
        if counts is None:
            counts = c
        else:
            for key in counts:
                counts[key] = [x + y for x, y in zip(counts[key], c[key])]

    print(counts)

    plot(timeouts, counts, total, benchmarks, curr_solvers)
