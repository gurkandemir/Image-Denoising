# Image-Denoising

In this project, we are expected to experience parallel programming with C/C++ using MPI
library. We implemented a parallel algorithm for image whose size is 200x200 denoising with the Ising model using
MetropolisHastings algorithm.

## Ising Model: 

The model consists of discrete variables that represent magnetic dipole
moments of atomic spins that can be in one of two states (+1 or -1). The spins are arranged in
a graph, usually a lattice, allowing each spin to interact with its neighbors. If we assume that a
black and white image is generated using The Ising model, then it means that if we take
random black pixel from the image, it is more likely that this pixel is surrounded by black
pixels (same for the white pixels).

## MetropolisHastings Algorithm:

Algoritm tells us which candidate Z image is more similar
to the original image. In order to reach the noise free image Z we need to make some
modification to noised image.

## Program Execution

Execute program using this command:
```
>mpic++ -std=c++11 -g main.cpp
```

After creating a executable, start the program with following command:
```
>mpiexec –oversubscribe -n [N] ./a.out [INPUT_FILE] [OUTPUT_FILE] [BETA] [PI]

WHERE
- N indicates that how many processors will run the program.
- INPUT_FILE indicates the noised image.
- OUTPUT_FILE indicates the where to print the result of program.
- BETA indicates the value of beta in order to implement MetropolisHastings algorithm.
- PI indicates the value of pi in order to implement MetropolisHastings algoritm.
```
