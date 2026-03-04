import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import LineCollection
from matplotlib.patches import Polygon as MplPolygon


class Viewer:
    def __init__(self, scene, start, goal):
        plt.ion()
        self.figure, self.axis = plt.subplots()
        try:
            plt.get_current_fig_manager().full_screen_toggle()
        except Exception:
            self.figure.set_size_inches(12, 12)

        self.euclid = float(np.linalg.norm(np.array(goal) - np.array(start)))

        self.axis.set_aspect("equal", adjustable="box")
        self.axis.set_xlim(0, scene.size)
        self.axis.set_ylim(0, scene.size)
        self.axis.set_xticks([])
        self.axis.set_yticks([])

        for obstacle in scene.obstacles:
            x, y = obstacle.exterior.xy
            self.axis.add_patch(
                MplPolygon(np.c_[x, y], closed=True, facecolor="blue", edgecolor="blue")
            )

        self.axis.plot([start[0]], [start[1]], "o", color="blue", markersize=8)
        self.axis.plot([goal[0]], [goal[1]], "o", color="lime", markersize=8)

        self.tree_lines = LineCollection([], colors="red", linewidths=0.5)
        self.axis.add_collection(self.tree_lines)

        (self.path_line,) = self.axis.plot([], [], color="lime", linewidth=2.5)
        self.title = self.axis.set_title("")
        self.figure.canvas.draw()
        self.figure.canvas.flush_events()

    def update(self, planner, iteration, total_iterations):
        self.tree_lines.set_segments(planner.edges())

        path = planner.best_path()
        if path is None:
            self.path_line.set_data([], [])
            self.title.set_text(
                f"Iter {iteration}/{total_iterations} | Nodes: {len(planner.nodes)} | Euclid: {self.euclid:.1f}"
            )
        else:
            self.path_line.set_data(path[:, 0], path[:, 1])
            self.title.set_text(
                f"Iter {iteration}/{total_iterations} | Nodes: {len(planner.nodes)} | "
                f"Cost: {planner.best_goal[0]:.1f} | Euclid: {self.euclid:.1f}"
            )

        self.figure.canvas.draw()
        self.figure.canvas.flush_events()
        plt.pause(0.001)

    def save_pdf(self, filename):
        self.figure.savefig(filename, format="pdf", bbox_inches="tight")
