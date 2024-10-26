# Experiments on Comparison of S5 Solvers

- Make sure that CEGARBox is running, by consulting the main README file.
- Before running, download and place all prerequisite files (listed below).
- To run all set of benchmarks, use:  
     ``./scripts/run_all.sh``
- To run a particular set of benchmark files (QS5, MQBF or 3CNF), use:  
    ``python3 scripts/run_benchmark.py <Benchmark_Name>``
- Since LWB files are large and cannot be generated at once, the script for it is different which only creates some files at a time. Use:  
    ``python3 scripts/run_lwb.py``
- After running the corresponding benchmark, results can be plotted using:  
    ``python3 scripts/plot.py <Benchmark_Name>``

## Required Files
To run the scripts, following files are required:

1. *S5Cheetah* - binary file for solver S5Cheetah
    - Link: http://www.square16.org/tools/s5cheetah/
    - Reference: Huang, Pei-Pei et al. “Solving the Satisfiability Problem of Modal Logic S5 Guided by Graph Coloring.” International Joint Conference on Artificial Intelligence (2019).

2. *S52SAT* - binary file for solver S52SAT
    - Link: http://www.cril.univ-artois.fr/~montmirail/s52SAT/
    - Reference: Caridroit, Thomas & Lagniez, Jean-Marie & Le Berre, Daniel & Lima, Tiago & Montmirail, Valentin. (2017). A SAT-Based Approach for Solving the Modal Logic S5-Satisfiability Problem. 

3. *KSP* - Resolution Calculus based Prover
    - Link: https://nalon.org/#software
    - Pattinson, D., Olivetti, N., Nalon, C. (2023). Resolution Calculi for Non-normal Modal Logics. In: Ramanayake, R., Urban, J. (eds) Automated Reasoning with Analytic Tableaux and Related Methods. TABLEAUX 2023. Lecture Notes in Computer Science(), vol 14278. Springer, Cham. https://doi.org/10.1007/978-3-031-43513-3_18
    - Note: place 

4. *LCKS5* - 
    - Link: Old link is down, use way back machine for this link: http://rsise.anu.edu.au/~rpg/LCKS5TabProver/lckS5Prover_r1368.tar
    - Abate, P., Goré, R. and Widmann, F., 2007, July. Cut-free single-pass tableaux for the logic of common knowledge. In Workshop on Agents and Deduction at TABLEAUX (Vol. 2007).

3. *QS5* - benchmark files
    - Link: https://drive.google.com/drive/folders/1BlUZ_Zm5jFzmkpYTjvvI_iyxx0gNQKPx
    - Note: Place extracted files in folder ./benchmarks/QS5

4. *MQBF* - benchmark files
    - Link: http://www.cril.univ-artois.fr/~montmirail/mosaic/#
    - Note: Download qbf, qbfL, qbfS, qbfMS, qbfML and place all files in one singular folder (i.e., no nested folders) ./benchmarks/MQBF

5. *3CNF* - benchmark files
    - Link: http://www.cril.univ-artois.fr/~montmirail/mosaic/#
    - Note: Place extracted files in folder ./benchmarks/3CNF

6. *LWB* - benchmark file generator
    - Link: https://github.com/cormackikkert/LWB-benchmark-generator
    - Use command: ``cd scripts && git clone https://github.com/cormackikkert/LWB-benchmark-generator``
