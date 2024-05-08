
# Project in DD2356: Vicsek Model for Flocking Birds
Project (graded, 3.0 credits) in the course DD2356 Methods in High Performance Computing (7.5 credits). Project goal is to make a parallelized implementations of the Vicsek Model using OpenMP and MPI, and comparing these with a serial verison. 

The implemented Vicsek model is on the form 

$$\mathbf{x}_i(t+\Delta t)=\mathbf{x}_i(t) + \mathbf{v}_i(t)\Delta t$$

where the velocity $\mathbf{v}_i(t)$ is of magnitude $v_0$ and with angle $\theta_i$ calculated through

$$\theta_i(t + \Delta t) = \langle \theta(t)\rangle_r + \Delta \theta$$

More information can be found in the Project Report. 

## Documentation
Doxygen with auto deployment has been setup in this repository. The documentation for this repository is available at [https://Teonord.github.io/DD2356-Project-Vicsek-Model/](https://Teonord.github.io/DD2356-Project-Vicsek-Model/)

## Files
- **proj_serial.c**
    - Contains serial implementation of the Vicsek model.
- **proj_omp.c**
    - Contains OpenMP threaded implementation of the Vicsek model.
- **proj_mpi.c**
    - Contains MPI implementation of the Vicsek model.
- **proj_mpi_omp.c**
    - Contains MPI implementation of the Vicsek model with OpenMP threading capabilities.
- **proj_common.h**
    - Contains all functions and constants (parameters) neccessary to all of the different implementations of the Vicsek model.
- **proj_tests.c**
    - Contains tests for all the functions defined in proj_common.h
- **plotter.py**
    - Python Matplotlib Quiver plotter for the results from the C code. 
- **verification_values.py**
    - Used to verify that all result values are the same when parameter VERIF is set to 1. Used to see that all of the different implementations of the code are consistent.

## Running Locally

This assumes you are running **Ubuntu** for local computer. 

Clone the project

```bash
  git clone https://github.com/Teonord/DD2356-Project-Vicsek-Model
```

Go to the project directory

```bash
  cd DD2365-Project-Vicsek-Model
```

Change parameters like number of birds and timestep in **proj_common.h**

```bash
  emacs proj_common.h
```

### Non MPI Implementations

Compile the files

```bash
  cc -o <filename>.out <filename>.c -fopenmp -lm
```

Set amount of OpenMP threads (if needed)

```bash
  export OMP_NUM_THREADS=<threads>
```

Run code

```bash
  ./<filename>.out > res
```

### For MPI Implementations

Install MPICH

```bash
  sudo apt install mpich
```

Compile the files

```bash
  mpicc -o <filename>.out <filename>.c -fopenmp -lm
```

Set amount of OpenMP threads (if needed)

```bash
  export OMP_NUM_THREADS=<threads>
```

Run code with number of processes

```bash
  mpiexec -n <processes> ./<filename>.out > res
```
## Running on Dardel
Follow guide in Running Locally to download and set up code.

Copy code to Dardel

```bash
  scp <DD2365-Project-Vicsek-Model> <username>@dardel.pdc.kth.se:Private/.
```

After entering Dardel and moving to proper folder, compile code.

```bash
   cc -o <filename>.out <filename>.c -fopenmp -lm
```

Create Job Script for running code.

```bash
   touch <filename>.sh
   emacs <filename>.sh
```

Enter information neccessary for your batched run of the code.

```bash
    #!/bin/bash -l

    #SBATCH -A <allocation_code>

    #SBATCH -J <filename>

    #SBATCH -p main

    #SBATCH -n <n>

    #SBATCH --nodes=<n>

    #SBATCH --ntasks-per-node=<n>

    #SBATCH --cpus-per-task=<n>

    #SBATCH -t 00:05:00

    export OMP_NUM_THREADS=<threads>
    export OMP_PLACES=cores

    srun ./<filename>.out > ./Results/res_<filename>
```

Save and exit using Ctrl-x, Ctrl-c, y, Enter.

Run the batched Job and wait for run to be completed.

```bash
   sbatch <filename>.sh
```

To check progress, you can check the queue.

```bash
   squeue -u <username>
```


## Visualization

This assumes that you have at least Python 3 installed

To visualize the code, take the results file and place it next to the plotter.py file. Change the "filename" variable to the name of the results file. 

Install neccessary modules 

```bash
   pip install matplotlib numpy
```

Run code 

```bash
   python ./plotter.py
```
## Testing
There are two tests of the code.

### Unit Testing 
To run unit testing of the code, follow the [Run Locally](#run-locally) section to compile the **proj_tests.c** file

Run the file 

```bash
  ./proj_tests.out
```

If output is all zeroes, code has passed the tests. If non-zero, look through the [code](../blob/master/proj_tests.c) to see what test hasn't been passed. Make sure that VERIF in **proj_commons.h** is set to 0 when running unit testing.

### Result Verification
To run result verification, first set VERIF to 1 in **proj_commons.h**. Do not change any other parameters. This will disable randomness and create a deterministic setup and effects for all functions. Next, follow [Run Locally](#run-locally) to run the **proj_serial.c** code and at least one other file you wish to verify. Put the resulting files in the folder with **verification_values.py**. Open **verification_values.py** and set "filename1" and "filename2" to the names of the result files.

Install neccessary modules 

```bash
   pip install numpy
```

Run code 

```bash
   python ./verification_values.py
```

Output should have one line with text. An otherwise empty file means verification was passed. Any more lines means something was problematic. If so, make sure that VERIF is set to 1.