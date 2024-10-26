import pandas as pd
import sys

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 checker.py <benchmark_name>")
        exit()

    bench = sys.argv[1]

    results_df = pd.read_csv(f"../results/{bench}/results.csv")

    results_df["incorrect"] = (
                            (results_df["CEGAR"] != results_df["Cheetah"]) &
                            (results_df["CEGAR"] != -1) &
                            (results_df["Cheetah"] != -1)
                            )
    
    incorrect_files = results_df[results_df["incorrect"]]["File"].tolist()

    if len(incorrect_files) == 0:
        print("All files are correct!")
    else:
        print("Incorrect files:")
        for f in incorrect_files:
            print(f"File: {f}, CEGAR: {results_df[results_df['File'] == f]['CEGAR'].values[0]}, Cheetah: {results_df[results_df['File'] == f]['Cheetah'].values[0]}, S52SAT: {results_df[results_df['File'] == f]['S52SAT'].values[0]}")