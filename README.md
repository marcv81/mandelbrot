# Mandelbrot

This project renders the Mandelbrot fractal to experiment with parallelism.

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

Benchmark rendering and save the image to JPEG.

    python3 benchmark.py

Explore the fractal interactively.

    python3 explore.py

## Benchmarks

Best render time of 5 attempts at different resolutions. CPU: Intel i7-8700, GPU: NVIDIA GTX 1060.

|       | CPU (threads) | GPU (CUDA) | CPU (OpenACC) | GPU (OpenACC) |
|-------|---------------|------------|---------------|---------------|
| 1080p | 25.7 ms       | 6.2 ms     | 24.5 ms       | 5.8 ms        |
| 4k    | 103.4 ms      | 22.0 ms    | 96.6 ms       | 22.6 ms       |
| 16k   | 1.614 s       | 269.7 ms   | 1.519 s       | 287.9 ms      |
