# Software Renderer
## Project Overview
This project's goal was to directly compare rendering techiniques used on the same scene. This allowed us to see the different pros and cons of each approach.

<p align="center">
<iframe width="560" height="315" src="https://www.youtube-nocookie.com/embed/6hbE6WlHI0E" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" allowfullscreen></iframe>
</p>

## Main features:
* Raytracing
* Rasterization
* Multi-threading
* Spatial Data Structures
* Binary Tree Serialization 
* .OBJ, .MTL File Parsing
* Interative Renders
* Scene

## Achievements
The goal of this project was to continue learning C++ while also creating a renderer. All the main features required for a First grade were hit although I took a detour from the assessment brief to pursue my interests in making the program more efficient.

Multi-threading was interesting to me as I could brute force speed up the raytracing rendering times. 
Bounding volume hierarchy increased the render times significantly but it took upto an hour to create the binary tree. 
Binary tree serialization was used to offset the initial cost of the BVH. Storing the binary trees in binary files meant that the computer no longer had to compute the BVH on each subsequent run. After the initial tree creation the BVH could be read in from a binary file in 1~2 minutes. 
