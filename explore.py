import ctypes
import pyglet

# Loads the shared library.
libmandelbrot = ctypes.cdll.LoadLibrary('./libmandelbrot.so')
libmandelbrot.render.argtypes = (ctypes.c_void_p,
    ctypes.c_int, ctypes.c_double, ctypes.c_double,
    ctypes.c_int, ctypes.c_double, ctypes.c_double)

# Pyglet window with an interactive Mandelbrot fractal.
class MandelbrotWindow(pyglet.window.Window):

    def __init__(self, x=-0.5, y=0.0, zoom=2.0, **kwargs):
        super().__init__(**kwargs)
        self.fps_display = pyglet.window.FPSDisplay(self)
        self.x = x
        self.y = y
        self.zoom = zoom
        buffer_size = 3 * self.width * self.height
        self.buffer = ctypes.create_string_buffer(buffer_size)

    # Renders the Mandelbrot fractal.
    def render(self):
        x_start = self.x - self.zoom
        x_stop = self.x + self.zoom
        y_start = self.y - self.zoom * self.height / self.width
        y_stop = self.y + self.zoom * self.height / self.width
        libmandelbrot.render(self.buffer, self.width, x_start, x_stop, self.height, y_start, y_stop)
        pyglet.image.ImageData(self.width, self.height, 'RGB', self.buffer).blit(0, 0)

    def on_draw(self):
        self.render()
        self.fps_display.draw()

    # Moves the view when the user drags the mouse.
    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
        if buttons & pyglet.window.mouse.LEFT:
            self.x -= 2.0 * dx / self.width * self.zoom
            self.y -= 2.0 * dy / self.width * self.zoom

    # Zooms in or out when the user scrolls the wheel.
    def on_mouse_scroll(self, x, y, scroll_x, scroll_y):
        self.zoom -= self.zoom * 0.05 * scroll_y

def update(dt):
    pass

window = MandelbrotWindow(fullscreen=True, vsync=0)
pyglet.clock.schedule_interval(update, 1/120.0)
pyglet.app.run()
