import math, random
import numpy as np
from shapely.geometry import Polygon, LineString, Point
from shapely.prepared import prep


class Scene:
    def __init__(self, size=100, num_obstacles=20, min_area=100, max_collisions=3):
        self.size = size
        self.num_obstacles = num_obstacles
        self.min_area = min_area
        self.max_collisions = max_collisions
        self.rng = random.Random()
        self.world = Polygon([(0, 0), (size, 0), (size, size), (0, size)])
        self.obstacles = []
        self.prepped = []

    def sample_start_goal(self, min_separation=50):
        while True:
            start = np.array(
                [self.rng.uniform(0, self.size), self.rng.uniform(0, self.size)], float
            )
            goal = np.array(
                [self.rng.uniform(0, self.size), self.rng.uniform(0, self.size)], float
            )
            if np.linalg.norm(start - goal) >= min_separation:
                return start, goal

    def _random_polygon(self):
        cx = self.rng.uniform(0, self.size)
        cy = self.rng.uniform(0, self.size)
        vertex_count = self.rng.randint(3, 6)
        angles = sorted(self.rng.uniform(0, 2 * math.pi) for _ in range(vertex_count))
        radius = self.rng.uniform(8, 18)
        points = [
            (cx + radius * math.cos(a), cy + radius * math.sin(a)) for a in angles
        ]
        return Polygon(points).convex_hull

    def build_obstacles(self, start, goal, attempts=20000):
        obstacles = []
        total_collisions = 0
        start_pt = Point(start)
        goal_pt = Point(goal)

        for _ in range(attempts):
            if len(obstacles) >= self.num_obstacles:
                break

            poly = self._random_polygon()
            if (
                (not poly.is_valid)
                or (poly.area < self.min_area)
                or (not self.world.contains(poly))
            ):
                continue
            if poly.intersects(start_pt) or poly.intersects(goal_pt):
                continue

            new_collisions = sum(poly.intersects(existing) for existing in obstacles)
            if total_collisions + new_collisions > self.max_collisions:
                continue

            obstacles.append(poly)
            total_collisions += new_collisions

        self.obstacles = obstacles
        self.prepped = [prep(p) for p in obstacles]

    def point_free(self, point):
        pt = Point(point)
        return not any(p.intersects(pt) for p in self.prepped)

    def segment_free(self, point_a, point_b):
        seg = LineString([point_a, point_b])
        return not any(p.intersects(seg) for p in self.prepped)
