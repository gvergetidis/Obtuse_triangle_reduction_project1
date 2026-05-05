# ProjectAlgorithms1

Detailed README for the first assignment in Software Development for Algorithmic Problems.

Group: 52

Members:

- Tagiar Korovesi, 1115202100068
- Giorgos Vergetidis, 1115202100014

Repository: <https://github.com/gvergetidis/ProjectAlgorithms1>

## Overview

This project implements a constrained Delaunay triangulation workflow for CG:SHOP-style JSON instances. It reads a geometric instance from a JSON file, constructs a constrained Delaunay triangulation with CGAL, applies local improvement techniques to reduce obtuse triangles inside the region boundary, and exports a solution JSON containing Steiner points and triangulation edges.

The main executable is named `triangulation`.

The implementation focuses on:

- parsing input instances with Boost.JSON;
- building a CGAL constrained Delaunay triangulation;
- respecting the polygonal region boundary and additional constraints;
- counting obtuse faces only inside the region boundary;
- trying edge flips when they are valid and useful;
- testing multiple Steiner point insertion strategies;
- exporting the final selected triangulation in JSON format.

## Dependencies

The project is written in C++ and uses:

- CMake;
- CGAL;
- GMP, through CGAL exact arithmetic support;
- Boost.JSON;
- Qt5 Widgets, Gui, Core, and OpenGL, used by `CGAL::draw`;
- a C++ compiler supported by the installed CGAL/CMake toolchain.

The CMake configuration searches for CGAL, Boost.JSON, and Qt5. If one of these dependencies is missing, configuration or compilation will fail.

## Repository Layout

```text
.
|-- Build/
|   `-- CMakeLists.txt
|-- include/
|   `-- CCDT.h
|-- src/
|   |-- lib.cpp
|   `-- triangulation.cpp
|-- tests/
|   |-- instance_*.json
|   `-- test_*.png
|-- test_instances/
|   |-- instance_test_*.json
|   `-- instance_test_*.png
|-- oldreame
`-- README.md
```

### Important Files

- `Build/CMakeLists.txt`: CMake build configuration. It creates the `triangulation` executable from `src/triangulation.cpp` and `src/lib.cpp`, links CGAL, Qt5, and Boost.JSON, and places the executable in `bin/`.
- `include/CCDT.h`: defines the project typedefs, helper prototypes, and the custom `CCDT` class.
- `src/lib.cpp`: contains helper geometry functions for obtuse detection, rational formatting, Steiner point calculation, and polygon-center insertion.
- `src/triangulation.cpp`: contains the program entry point, JSON parsing, triangulation construction, improvement pipeline, and JSON export.
- `tests/` and `test_instances/`: contain example input instances and related images.
- `oldreame`: preserved copy of the previous README.

## Build Instructions

From the repository root, configure CMake with:

```bash
cmake -S Build -B Build
```

Then compile with:

```bash
make -C Build
```

The configured runtime output directory is `bin/`, so the executable is expected at:

```text
bin/triangulation
```

On Windows or other generators, the exact build command may differ depending on the selected CMake generator.

## Usage

The executable expects exactly one command-line argument: the path to an input JSON instance.

Example:

```bash
./bin/triangulation test_instances/instance_test_1.json
```

If the argument is missing, the program prints a usage message and exits.

The program also opens CGAL drawing windows at several stages because `CGAL::draw` is called during the workflow. A graphical environment with the required Qt support is therefore expected.

## Input JSON Format

The input file is expected to contain:

```json
{
  "instance_uid": "test_1",
  "num_points": 10,
  "points_x": [0, 4, 12],
  "points_y": [0, 0, -6],
  "region_boundary": [0, 1, 2],
  "num_constraints": 0,
  "additional_constraints": []
}
```

Fields:

- `instance_uid`: string identifier for the instance.
- `num_points`: declared number of points.
- `points_x`: x-coordinates of the input points.
- `points_y`: y-coordinates of the input points.
- `region_boundary`: indices of the points that form the polygonal boundary.
- `num_constraints`: declared number of additional constrained edges.
- `additional_constraints`: array of index pairs, where each pair defines an additional constrained edge.

The program validates that:

- the number of x-coordinates and y-coordinates matches `num_points`;
- the number of parsed constraints matches `num_constraints`;
- each additional constraint contains exactly two point indices.

## Output JSON Format

The program writes its output to:

```text
out/output.json
```

The exported JSON contains:

```json
{
  "content_type": "CG_SHOP_2025_Solution",
  "instance_uid": "test_1",
  "steiner_points_x": [],
  "steiner_points_y": [],
  "edges": []
}
```

Fields:

- `content_type`: fixed solution type string.
- `instance_uid`: copied from the input instance.
- `steiner_points_x`: x-coordinates of exported Steiner points.
- `steiner_points_y`: y-coordinates of exported Steiner points.
- `edges`: triangulation edges represented as point-index pairs.

Integer Steiner coordinates are written as JSON numbers. Rational coordinates are written as strings in `numerator/denominator` form.

## Algorithm Summary

### 1. Input Parsing

`src/triangulation.cpp` reads the input file into a string and parses it with Boost.JSON. The coordinates are stored in separate `X` and `Y` vectors, then combined into CGAL `Point` objects.

The region boundary is stored as a list of point indices. Boundary edges are also inserted as constraints so the triangulation respects the input polygon.

### 2. Custom Constrained Delaunay Triangulation

`include/CCDT.h` defines `CCDT`, a custom class derived from CGAL's constrained Delaunay triangulation type.

The class adds:

- a `region_boundary` polygon;
- `insert_no_flip` methods, which insert points without the usual flip-around behavior;
- `point_in_region_boundary`, which checks whether a point is inside or on the boundary polygon;
- `face_in_region_boundary`, which checks whether a face and its centroid are inside the boundary;
- `obtuse_angles_count`, which counts obtuse finite faces inside the valid region.

The project typedefs this class as `CDT`.

### 3. Initial Triangulation

The program inserts all input points, then inserts:

- additional constraints from `additional_constraints`;
- all region-boundary edges as constraints.

The region-boundary points are also stored in the `region_boundary` polygon used for geometric filtering.

### 4. Edge Flip Pass

The program copies the initial triangulation and scans finite edges. An edge is considered for flipping only when:

- it is not constrained;
- both adjacent faces are finite;
- both adjacent faces are inside the region boundary;
- at least one adjacent face is obtuse;
- the quadrilateral formed by the adjacent faces is convex;
- the flip would not create an invalid collinear configuration.

After the pass, the program compares the number of obtuse faces before and after flipping. If flipping improved the count, the improved triangulation is used as the base for the Steiner point methods. Otherwise, the original triangulation is used.

### 5. Steiner Point Strategies

The program tests four Steiner point insertion strategies. Each strategy is applied to a separate copy of the selected base triangulation.

The iteration limit is controlled by:

```cpp
#define limit 20
```

Strategies:

- Middle point: inserts the midpoint of the side opposite an obtuse angle.
- Circumcenter/centroid: inserts the circumcenter of an obtuse triangle, or the centroid if the circumcenter is outside the region boundary.
- Polygon center: searches for neighboring obtuse faces without constraints, forms a convex polygon when possible, and inserts a point at the polygon center.
- Projection: projects the obtuse vertex onto the opposite side and inserts that projection point.

The implementation exports the projection-based result, because this method was observed to give the best results in the existing project notes.

## Helper Functions

`src/lib.cpp` provides the main geometry helper functions:

- `obtuse_face`: checks whether a face has an obtuse angle.
- `get_obtuse_ind`: returns the vertex index of the obtuse angle.
- `print_rational`: converts an exact CGAL coordinate into numerator and denominator strings.
- `get_projection`: computes the projection of the obtuse vertex onto the opposite side.
- `get_middlepoint`: computes the midpoint of the side opposite the obtuse angle.
- `get_centroid`: computes the centroid of a face.
- `get_circumcenter`: computes the circumcenter of a face.
- `get_circumcenter_centroid`: chooses circumcenter or centroid depending on boundary containment.
- `face_has_constraint`: checks whether a face has constrained edges.
- `polygon_center`: inserts a Steiner point at the center of a valid convex polygon formed by neighboring obtuse faces.

## Notes and Limitations

- The program writes to `out/output.json`; the `out/` directory must exist before writing succeeds.
- Several `CGAL::draw` calls are present, so running the executable is interactive/graphical.
- The exported result currently comes from the projection strategy.
- The program stores only projection-method Steiner coordinates in the output arrays.
- The code uses exact CGAL constructions, so rational output coordinates may be large.
- The project keeps the previous README as `oldreame`.

## Suggested Development Workflow

1. Add or edit input instances in `tests/` or `test_instances/`.
2. Configure and build with CMake.
3. Run the executable with a JSON instance path.
4. Inspect the generated `out/output.json`.
5. Compare obtuse-angle counts printed by the program for each method.
