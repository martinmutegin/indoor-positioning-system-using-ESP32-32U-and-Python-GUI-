import matplotlib.pyplot as plt
import matplotlib.animation as animation
import tkinter as tk
import math
from tkinter import ttk
from config import ANCHORS
from matplotlib.patches import Circle
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg


class TagVisualizer:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Tag Position Visualizer")
        self.root.geometry("1000x900")

        self.fig, self.ax = plt.subplots(figsize=(10, 10))
        self.canvas = None
        self.tag_pos = (None, None)
        self.distances = {}
        self.anchors = ANCHORS
        self.circles = {}  # <-- To hold Circle patches

        self.setup_plot()
        self.coord_label = ttk.Label(self.root, text="Tag Coordinates: N/A", font=("Arial", 12))
        self.coord_label.pack(pady=10)
        self.ani = animation.FuncAnimation(self.fig, self.update_plot, interval=500)

    def setup_plot(self):
        self.ax.clear()
        self.ax.set_xlim(-3, 10)
        self.ax.set_ylim(-3, 10)
        self.ax.set_xlabel("X (meters)")
        self.ax.set_ylabel("Y (meters)")
        self.ax.set_title("Anchor Positions and Tag Location")
        self.ax.grid(True)

        for name, (x, y) in self.anchors.items():
            self.ax.plot(x, y, 'bs')  # anchors in blue squares
            self.ax.text(x, y + 0.1, name, ha='center')

            # Add empty placeholder circles
            circle = Circle((x, y), 0.01, fill=False, linestyle='--', edgecolor='gray', alpha=0.5)
            self.circles[name] = circle
            self.ax.add_patch(circle)

        # Draw dashed triangle connecting anchors
        x_vals = [v[0] for v in self.anchors.values()] + [list(self.anchors.values())[0][0]]
        y_vals = [v[1] for v in self.anchors.values()] + [list(self.anchors.values())[0][1]]
        self.ax.plot(x_vals, y_vals, 'b--', alpha=0.3)

        self.tag_plot, = self.ax.plot([], [], 'ro', label="Tag")
        self.ax.legend()
        self.fig.tight_layout()

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.get_tk_widget().pack()

    def update_tag_position(self, x, y, distances=None):
        self.tag_pos = (x, y)
        if distances:
            self.distances = distances

    def update_plot(self, frame):
        x, y = self.tag_pos
        if x is not None and y is not None and not (math.isnan(x) or math.isnan(y)):
            self.tag_plot.set_data([x], [y])
            self.coord_label.config(text=f"Tag Coordinates: ({x:.2f}, {y:.2f})")
        else:
            self.tag_plot.set_data([], [])
            self.coord_label.config(text="Tag Coordinates: N/A")

        # Update circles with latest distances
        for name, circle in self.circles.items():
            anchor_x, anchor_y = self.anchors[name]
            radius = self.distances.get(name, 0.01)
            circle.set_radius(radius)

        self.canvas.draw_idle()

    def run(self):
        self.root.mainloop()
