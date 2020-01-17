import ctypes
import PIL.Image
import time

# Loads the shared library.
libmandelbrot = ctypes.cdll.LoadLibrary('./libmandelbrot.so')
libmandelbrot.render.argtypes = (ctypes.c_void_p,
    ctypes.c_int, ctypes.c_double, ctypes.c_double,
    ctypes.c_int, ctypes.c_double, ctypes.c_double)

# Renders the image 5 times in 1080p, 4k, and 16k.
for width, height in (1920, 1080), (3840, 2160), (15360, 8640):
    buffer_size = 3 * width * height
    buffer = ctypes.create_string_buffer(buffer_size)
    for _ in range(5):
        start_time = time.monotonic()
        libmandelbrot.render(buffer, width, -2.5, 1.5, height, -2.0 * height / width, 2.0 * height / width)
        stop_time = time.monotonic()
        print('%d x %d rendered in %.1f ms.' % (width, height, 1000 * (stop_time - start_time)))
    image = PIL.Image.frombytes('RGB', (width, height), buffer, 'raw')
    image.save('mandelbrot-%dx%d.jpeg' % (width, height))
