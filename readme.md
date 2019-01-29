# Performance Analysis on Data Interleaving in OpenGl


### Introduction

This readme is part of a personal project investigating how data formats passed to the GPU though the OpenGl API affect drawing performance of triangles in a height map. 
More specifically how multiple array buffers performs against data interleaving in one array buffer. The results are split up into three different tests on two different data formats. \
The first is rendering with just a normal, the second is with a reasonably small texture that fits within the eDRAM on my Intel Iris Plus Graphics card, and lastly a large texture that should cause lots of cache misses and doesn’t fit within that eDRAM.


### Construction of Height Map

The construction of the height map is fairly simple. 
I started by first constructing a 64X64 grid out of triangles on the x-z axis. 
Then I varied the height of the vertices by inputting the location of the grid into the sin function, .5*sin(i) + 2*sin(j). 
This general height map is used for all of the performance comparisons


### Running the Code

I tested each variation of data interleaving vs multiple array buffers on different amounts of triangles, still within a 64X64 grid. 
Each frame/second measure in the performance graph is the average of the rendering time for 1000 individual frames. 


### Height Map with Normals

The following image is a height map that is colored with the direction that the normal faces. 
Underneath that height map is a graph of our two samples. 
The interleaved texture drew about 1.08X times as fast as the OpenGL draw call with two array buffers. 
Not a significant speed up but can still be seen. 
The difference in the speeds could be attributed to not having to access two separate locations in memory, and instead accessing the vertex and normal sequentially. 
In addition the total amount of data (589.8 Kb in float data for 8912 triangles and 17.7 MB for 294912 triangles) doesn’t fit completely within the L1 and L2 caches but fits within my graphics card eDRAM. 
The interleaved data might increase caching performance, decreasing cache line evictions.

![alt text](https://raw.githubusercontent.com/boonemiller/GraphicsPerformance/master/GraphicsPerformance/readmeImages/NormalMap.png)
Height Map with just normals coloring the triangles

![alt text](https://raw.githubusercontent.com/boonemiller/GraphicsPerformance/master/GraphicsPerformance/readmeImages/MoreTrianglesNormal.png)
Similar height map, just made using more triangles


### Height Map with Small Texture

The following image is the same height map but with a .bmp texture that comes from my Ray Tracer implementation in another repo (https://github.com/boonemiller/Ray-Tracer). 
Like before, I compared data interleaving against 2 array buffers, this time one buffer had vertex data and the other contained uv coordinates. 
I found similar results with the small texture as with the height map with normals. 
The interleaved data draw call is faster than the 2 buffer draw call, this time with a 1.11X speed up. 
Like before I can probably attribute the speed up to better caching and not having to have two separate memory accesses to the two array buffer locations. 
Even with the 778Kb texture, everything still fits within the 64 MB eDRAM cache on my graphics card.

![alt text](https://raw.githubusercontent.com/boonemiller/GraphicsPerformance/master/GraphicsPerformance/readmeImages/smallTexture.png)
Height map using a small texture.


### Height Map with Large Texture

I repeated the process a third time, except this time with a very large .bmp texture I made using random rgb values. 
This time the texture itself is larger than both caches and the 64 MB eDRAM cache. 
This 286MB texture dramatically reduced the rendering speed, starting out by taking 4.12X times as long to draw as compared to the small texture on the lowest amount of triangles.
This slowdown can probably be attributed due to the fact that at some point the gpu needs to go out to main memory and cache in texture data from system RAM (integrated graphics card). 
While the small texture doesn’t need to do this data fetch.

The small texture starts to catch up to the large texture the more triangles I draw, only being 1.52X times as fast when I draw a lot of triangles. 
I experienced a 1.01X speed up with interleaving data, however this is very minimal as compared to the other tests. 
This lower number is probably due to the texture fetching being the bottleneck on the draw call, and not the vertex data fetch.

![alt text](https://raw.githubusercontent.com/boonemiller/GraphicsPerformance/master/GraphicsPerformance/readmeImages/largeTexture.png)
Height map using a large texture of random rgb values.


### Results

The results, as represented in frames/seconds. 
Show the trend that data interleaving results in some amount of speed up on the overall OpenGl drawing time. 
The large texture represented on the graph is hard to see because their drawing times were so similar the two lines overlap.

![alt text](https://raw.githubusercontent.com/boonemiller/GraphicsPerformance/master/GraphicsPerformance/readmeImages/PerformanceGraph.png)


### Conclusions

Data interleaving leads to marginal speedups on the height maps and textures I tested it on as compared to multiple array buffers. 
Keeping textures small could also affect performance as seen in the small vs. large texture drawing times. 
Seeing a 4.12X slow down with the large texture on the small number of triangles test. 
I think if I had more vertex buffer data, I might see larger speed ups in drawing times. 
Or if I had multiple VBOs for more complex scenes I might see improvements with data interleaving.


Special thanks to http://www.opengl-tutorial.org for a lot of the code I used and the code that is in the repo. 
