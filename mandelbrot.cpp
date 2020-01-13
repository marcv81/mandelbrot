#include <cstdint>
#include <thread>

// Algorithm parameters.
const float escape_radius = 2.5;
const int max_iterations = 60; // Must be divisible by 3.
const int threads_size = 12;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_t;

// Counts the number of iterations for the series to diverge.
uint8_t count_iterations(float x, float y) {
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
float scale(int index, int size, float start, float stop)
{
    return (stop - start) * index / size + start;
}

// Paints the pixel at coordinates (x_index, y_index) of an image buffer.
void paint(uint8_t* buffer, int x_index, int y_index, int x_size, color_t color) {
    int index = 3 * (x_index + (y_index * x_size));
    buffer[index] = color.red;
    buffer[index+1] = color.green;
    buffer[index+2] = color.blue;
}

// Converts a number of iterations to a RGB color.
color_t palette(uint8_t iterations) {
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
void render_thread(uint8_t *buffer, int thread,
    int x_size, float x_start, float x_stop,
    int y_size, float y_start, float y_stop)
{
    for (int y_index=thread; y_index<y_size; y_index+=threads_size) {
        float y = scale(y_index, y_size, y_start, y_stop);
        for (int x_index=0; x_index<x_size; x_index++) {
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
    std::thread threads[threads_size];
    for (int i=0; i<threads_size; i++) {
        threads[i] = std::thread(render_thread, buffer, i,
            x_size, x_start, x_stop,
            y_size, y_start, y_stop);
    }
    for (int i=0; i<threads_size; i++) {
        threads[i].join();
    }
}
