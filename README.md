# Intro

This project renders the Mandelbrot fractal.

## Compile

Compile for CPU using standard library threads.

    g++ -std=c++11 -fPIC -o libmandelbrot.so --shared mandelbrot-threads.cpp

Compile for GPU using CUDA.

    nvcc --compiler-options '-fPIC' -o libmandelbrot.so --shared mandelbrot.cu

Compile for CPU using OpenACC.

    pgc++ -std=c++11 -fPIC -o libmandelbrot.so -shared -ta=multicore mandelbrot-openacc.cpp

Compile for GPU using OpenACC.

    pgc++ -std=c++11 -fPIC -o libmandelbrot.so -shared -ta=nvidia mandelbrot-openacc.cpp

## Run

Render and save to JPEG.

    python3 render.py

Explore the fractal interactively.

    python3 explore.py
