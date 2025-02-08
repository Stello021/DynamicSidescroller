# Dynamic Side-scroller
## Player Movement
Player movement is limited to following a not linear spline as path.
It means that it can move dynamically around objects 
or through a 3D space maintains a side-scroller view.
### How it works
After reference the **Spline Actor**, Character needs to finds
the movement orientation.
At this purpose, it's used the **normalized tangent** of the closest spline's point to character.

![Tangent.png](Documentation/Media/Tangent.png)

This vector is multiplied for **input value** (`float`)  to determine whether the vector is positive or not,
and for a **scan value** that indicates magnitude and how long to following that.
This is necessary to know how the actor must be rotated to reach the next spline's point that means **vector direction**
and where it points.
All this calculation ignored Z-value because is not dependent from path, allowing a free jumping and not slowdowns when 
player has a height difference from spline. To make that is used the method `GetSafeNormal2D()`.
When spline is **not a closed loop** there could be **floating precision issues** when character reaches **boundaries**.
To fix it, before movement is calculated, actor location is checked to allows movement only in the opposite direction.

![Movement Example.gif](Documentation/Media/Movement%20Example.gif)