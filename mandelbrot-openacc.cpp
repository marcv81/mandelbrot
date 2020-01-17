#include <cstdint>

// Algorithm parameters.
const double escape_radius = 2.5;
const int max_iterations = 60; // Must be divisible by 3.

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} color_t;

// Counts the number of iterations for the series to diverge.
uint8_t count_iterations(double x, double y) {
    uint8_t iterations = 0;
    double u = 0.0, v = 0.0;
    while (iterations < max_iterations && (u * u + v * v < escape_radius * escape_radius)) {
        double _u = u * u - v * v + x;
        double _v = 2 * u * v + y;
        u = _u;
        v = _v;
        iterations++;
    }
    return iterations;
}

// Linerarly scales the interval [0:size] to [start:stop].
double scale(int index, int size, double start, double stop)
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

// Renders the entire image buffer.
extern "C" void render(uint8_t *buffer,
    int x_size, double x_start, double x_stop,
    int y_size, double y_start, double y_stop)
{
    #pragma acc kernels copyout(buffer[0:3*x_size*y_size])
    for (int y_index=0; y_index<y_size; y_index++) {
        double y = scale(y_index, y_size, y_start, y_stop);
        for (int x_index=0; x_index<x_size; x_index++) {
            double x = scale(x_index, x_size, x_start, x_stop);
            uint8_t iterations = count_iterations(x, y);
            color_t color = palette(iterations);
            paint(buffer, x_index, y_index, x_size, color);
        }
    }
}
