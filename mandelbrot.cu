#include <cstdint>

// Algorithm parameters.
const float escape_radius = 2.5;
const int max_iterations = 60; // Must be divisible by 3.
const int blocks_size_x = 8;
const int blocks_size_y = 10;
const int threads_size_x = 32;
const int threads_size_y = 32;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_t;

// Counts the number of iterations for the series to diverge.
__device__ uint8_t count_iterations(float x, float y) {
    uint8_t iterations = 0;
    float u = 0.0, v = 0.0;
    while (iterations < max_iterations && (u * u + v * v < escape_radius * escape_radius)) {
        float _u = u * u - v * v + x;
        float _v = 2 * u * v + y;
        u = _u;
        v = _v;
        iterations++;
    }
    return iterations;
}

// Linerarly scales the interval [0:size] to [start:stop].
__device__ float scale(int index, int size, float start, float stop)
{
    return (stop - start) * index / size + start;
}

// Paints the pixel at coordinates (x_index, y_index) of an image buffer.
__device__ void paint(uint8_t* buffer, int x_index, int y_index, int x_size, color_t color) {
    int index = 3 * (x_index + (y_index * x_size));
    buffer[index] = color.red;
    buffer[index+1] = color.green;
    buffer[index+2] = color.blue;
}

// Converts a number of iterations to a RGB color.
__device__ color_t palette(uint8_t iterations) {
    uint8_t red = 0, green = 0, blue = 0;
    if (iterations < max_iterations) {
        if (iterations < max_iterations / 3) {
            red = 255 * 3 * iterations / max_iterations;
        } else if (iterations < 2 * max_iterations / 3) {
            red = 255;
            green = 255 * 3 * (iterations - (max_iterations / 3)) / max_iterations;
        } else {
            red = 255;
            green = 255;
            blue = 255 * 3 * (iterations - (2 * max_iterations / 3)) / max_iterations;
        }
    }
    return { red, green, blue };
}

// Partially renders the image buffer for a single thread.
__global__ void render_thread(uint8_t *buffer,
    int x_size, float x_start, float x_stop,
    int y_size, float y_start, float y_stop)
{
    int y_first = threadIdx.y + (blockIdx.y * blockDim.y);
    int y_step = blockDim.y * gridDim.y;
    for (int y_index=y_first; y_index<y_size; y_index+=y_step) {
        float y = scale(y_index, y_size, y_start, y_stop);
        int x_first = threadIdx.x + (blockIdx.x * blockDim.x);
        int x_step = blockDim.x * gridDim.x;
        for (int x_index=x_first; x_index<x_size; x_index+=x_step) {
            float x = scale(x_index, x_size, x_start, x_stop);
            uint8_t iterations = count_iterations(x, y);
            color_t color = palette(iterations);
            paint(buffer, x_index, y_index, x_size, color);
        }
    }
}

// Renders the entire image buffer.
extern "C" void render(uint8_t *buffer,
    int x_size, float x_start, float x_stop,
    int y_size, float y_start, float y_stop)
{
    uint8_t* gpu_buffer;
    size_t size = sizeof(uint8_t) * 3 * x_size * y_size;
    cudaMalloc(&gpu_buffer, size);
    render_thread<<<
        dim3(blocks_size_x, blocks_size_y),
        dim3(threads_size_x, threads_size_y)>>>(gpu_buffer,
            x_size, x_start, x_stop,
            y_size, y_start, y_stop);
    cudaMemcpy(buffer, gpu_buffer, size, cudaMemcpyDeviceToHost);
    cudaFree(gpu_buffer);
}
