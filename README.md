Some OpenGL programs from Computer Graphics course assignments at University of Bologna.

![](https://i.imgur.com/xsZ8Y3u.png)

Every project choosen + additional improvement not required that i added:
- Project 1: Bézier curves drawing:
  - istead of using an array[] for control points i used an std::vector<>. In this way it's possible to have infinite control points.
  - curve rendering is done with adaptive subdivision
- Project 3: Interactive navigation in a 3d scene
- Project 6: Lighting, shading & texture mapping
    - Started from code of project 3, so camera animation is included
    - With key 't' you can change texture of selected object, with 'm' you can change the shader
    - Procedural texture is implemented with a Julia fractal (you can generate new one pressing 'f' )


I would like to thanks for the help [Serena Morigi](http://www.dm.unibo.it/~morigi/), [Matteo Berti](https://github.com/methk) and [Devid Farinelli](https://github.com/misterdev)
