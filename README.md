## About this project  
This started with me being frustrated by Geogebra's 3D mode. I found it difficult 
to visualize and understand exactly what was happening. If it had VR support, it 
would be great, but to my knowledge no "Geogebra 3D but with VR" software exists.  
Eventually, this project is supposed to become exactly that. A fully VR capable 
environment for visualizing geometric relationships and constructions in "real"
space.  

## Current status  
A lot of the framework around the geometry itself is in place, with a handful of 
shapes currently implemented. Next steps include implementing more basic shapes, 
measurement read-outs (e.g. angle or length measurements), and abstracting the 
camera operations into a separate class.  

## About the implementation
Shapes are the base geometries like points, lines, circles, etc. which make up
the scene. Points are the most basic, and can be found at the root of every
other shape's parent tree (parallel lines are defined by a line and a point to
pass through, the line is then defined by two more points; circles are defined
by a center point and a tangent point). Most shapes are categorized as "like"
a shape. Lines, rays, and segments are all line-like. Points and midpoints are
both point-like. This is done by inheriting a "GenericLine" or similar class in
the specific shape's class and ensures that for example a line can be made using
any combination of types of points (e.g. a midpoint and an ordinary point).  
Some shapes can be manipulated by clicking and dragging, or through the GUI
implemented using [imgui](https://github.com/ocornut/imgui). Doing either will
update the shape itself, as well as any descendants which rely on the properties
of the shape.  
To implement most new shapes, only a few things need to be defined:
- a new enum entry for the new shape type
- any manipulable properties (e.g. position, distance along line)
- what types of parent objects it requires (e.g. line requires two point-like
  parents)
- visual representation and how to calculate it
- ray intersection detection (used for selection, may change for different
  control methods in the future)
- serialization/deserialization

The appropriately inherited base classes handle most of the other required
functionality.  

Serialization is done through nlohmann's [JSON for Modern C++](https://github.com/nlohmann/json)
library. It allows for saving and loading all of a scene's shapes in a
human-readable format

## Plans for future progress
Work on this project has mostly stopped since I started a part-time job, and I've
instead been working on projects that are more helpful in my day-to-day life. I do
hope to get this into a fully functional state in the future.  

ty for reading, have a great day
