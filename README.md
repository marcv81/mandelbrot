# Intro

This project renders the Mandelbrot fractal.

Compile for CPU.

    g++ -std=c++11 -fPIC -o libmandelbrot.so --shared mandelbrot.cpp

Compile for GPU using CUDA.

    nvcc --compiler-options '-fPIC' -o libmandelbrot.so --shared mandelbrot.cu

Run.

    python3 render.py
