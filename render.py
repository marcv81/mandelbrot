import ctypes
import PIL.Image
import time

width, height = 12000, 8000

# Loads the shared library.
libmandelbrot = ctypes.cdll.LoadLibrary('./libmandelbrot.so')
libmandelbrot.render.argtypes = (ctypes.c_void_p,
    ctypes.c_int, ctypes.c_float, ctypes.c_float,
    ctypes.c_int, ctypes.c_float, ctypes.c_float)

# Renders the image.
buffer_size = 3 * width * height
buffer = ctypes.create_string_buffer(buffer_size)
start_time = time.monotonic()
libmandelbrot.render(buffer, width, -2.5, 1.5, height, -2.0 * height / width, 2.0 * height / width)
stop_time = time.monotonic()
print('%d x %d rendered in %.1f ms.' % (width, height, 1000 * (stop_time - start_time)))

# Saves the image.
image = PIL.Image.frombytes('RGB', (width, height), buffer, 'raw')
image.save('mandelbrot.jpeg')
