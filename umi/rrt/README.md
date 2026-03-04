# RRT* implementation and visualization
This repo implements RRT* in a 100x100 square, with randomly generated start, end and obstacles. It is enforced that the start and end aren't reachable via straight line.

<object data="rrt_star_result.pdf" type="application/pdf" width="100%" height="600px"></object>

To run the code, use uv.
```bash
uv run main.py
```
This will open an interactive matplitlib visualisation and export the final path as a vector graphic.

One such visualization is in the repo as `rrt_star_result.pdf`.