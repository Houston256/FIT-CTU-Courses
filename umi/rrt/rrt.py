import random
import numpy as np


class RRTStar:
    def __init__(
        self,
        scene,
        start,
        goal,
        step_size=3,
        near_radius=6,
        goal_radius=4,
        goal_bias=0.05,
    ):
        self.scene = scene
        self.start = np.array(start, float)
        self.goal = np.array(goal, float)

        self.step_size = step_size
        self.near_radius = near_radius
        self.goal_radius = goal_radius
        self.goal_bias = goal_bias
        self.rng = random.Random()

        self.nodes = np.array([self.start], float)
        self.parents = [-1]
        self.costs = [0.0]
        self.children = [[]]

        self.goal_candidates = set()
        self.best_goal = None  # (total_cost_to_goal, node_index)

    def sample(self):
        if self.best_goal is None and self.rng.random() < self.goal_bias:
            return self.goal.copy()
        return np.array(
            [
                self.rng.uniform(0, self.scene.size),
                self.rng.uniform(0, self.scene.size),
            ],
            float,
        )

    def nearest_index(self, point):
        return int(np.argmin(np.linalg.norm(self.nodes - point, axis=1)))

    def near_indices(self, point):
        return np.where(np.linalg.norm(self.nodes - point, axis=1) <= self.near_radius)[
            0
        ].tolist()

    def steer(self, from_point, to_point):
        direction = to_point - from_point
        length = float(np.linalg.norm(direction))
        if length <= self.step_size:
            return to_point
        return from_point + (self.step_size / length) * direction

    def reparent(self, child_index, new_parent_index):
        old_parent = self.parents[child_index]
        if old_parent != -1:
            if child_index in self.children[old_parent]:
                self.children[old_parent].remove(child_index)
        self.parents[child_index] = new_parent_index
        self.children[new_parent_index].append(child_index)

    def update_subtree_costs(self, root_index):
        stack = [root_index]
        while stack:
            node_index = stack.pop()
            for child_index in self.children[node_index]:
                self.costs[child_index] = self.costs[node_index] + float(
                    np.linalg.norm(self.nodes[child_index] - self.nodes[node_index])
                )
                stack.append(child_index)

    def recompute_best_goal(self):
        best = None
        for node_index in self.goal_candidates:
            total = self.costs[node_index] + float(
                np.linalg.norm(self.goal - self.nodes[node_index])
            )
            if best is None or total < best[0]:
                best = (total, node_index)
        self.best_goal = best

    def best_path(self):
        if self.best_goal is None:
            return None
        _, goal_parent = self.best_goal
        points = [self.goal]
        current = goal_parent
        while current != -1:
            points.append(self.nodes[current])
            current = self.parents[current]
        return np.array(points[::-1])

    def edges(self):
        if len(self.nodes) <= 1:
            return []
        parent_array = np.asarray(self.parents[1:], dtype=int)
        mask = parent_array >= 0
        if not np.any(mask):
            return []
        return np.stack([self.nodes[parent_array[mask]], self.nodes[1:][mask]], axis=1)

    def step_once(self):
        x_rand = self.sample()
        nearest = self.nearest_index(x_rand)
        x_new = self.steer(self.nodes[nearest], x_rand)

        if not (0 <= x_new[0] <= self.scene.size and 0 <= x_new[1] <= self.scene.size):
            return
        if not self.scene.point_free(x_new):
            return
        if not self.scene.segment_free(self.nodes[nearest], x_new):
            return

        near_set = self.near_indices(x_new)

        self.nodes = np.vstack([self.nodes, x_new])
        new_index = len(self.nodes) - 1
        self.parents.append(-1)
        self.costs.append(float("inf"))
        self.children.append([])

        best_parent = nearest
        best_cost = self.costs[nearest] + float(
            np.linalg.norm(x_new - self.nodes[nearest])
        )

        for neighbor in near_set:
            if not self.scene.segment_free(self.nodes[neighbor], x_new):
                continue
            candidate = self.costs[neighbor] + float(
                np.linalg.norm(x_new - self.nodes[neighbor])
            )
            if candidate < best_cost:
                best_cost = candidate
                best_parent = neighbor

        self.reparent(new_index, best_parent)
        self.costs[new_index] = best_cost

        for neighbor in near_set:
            if neighbor == best_parent:
                continue
            if not self.scene.segment_free(x_new, self.nodes[neighbor]):
                continue
            rewired = self.costs[new_index] + float(
                np.linalg.norm(self.nodes[neighbor] - x_new)
            )
            if rewired < self.costs[neighbor]:
                self.reparent(neighbor, new_index)
                self.costs[neighbor] = rewired
                self.update_subtree_costs(neighbor)

        if np.linalg.norm(
            x_new - self.goal
        ) <= self.goal_radius and self.scene.segment_free(x_new, self.goal):
            self.goal_candidates.add(new_index)

        if self.goal_candidates:
            self.recompute_best_goal()
