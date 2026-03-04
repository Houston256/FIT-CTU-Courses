import matplotlib.pyplot as plt
from scene import Scene
from rrt import RRTStar
from viewer import Viewer


class Simulation:
    def __init__(self, iterations=5_000, draw_every=25):
        self.iterations = iterations
        self.draw_every = draw_every

    def run(self):
        while True:
            scene = Scene()
            start, goal = scene.sample_start_goal()
            scene.build_obstacles(start, goal)
            if not scene.segment_free(start, goal):
                break

        planner = RRTStar(scene, start, goal)
        viewer = Viewer(scene, start, goal)

        for iteration in range(self.iterations):
            planner.step_once()
            if iteration % self.draw_every == 0:
                viewer.update(planner, iteration, self.iterations)

        viewer.update(planner, self.iterations, self.iterations)
        viewer.save_pdf("rrt_star_result.pdf")
        plt.ioff()
        plt.show()


if __name__ == "__main__":
    Simulation().run()
