# TeamRoadAnt
![Algernon]()
Code and documentation for a MicroMouse team with Jigar Hira, Kevin Chian, Tritai Nguyen, Dominic Carone, and Dylan Kao of UC Irvine. MicroMouse is a competition hosted by IEEE where small, 25x25cm autonomous robots are made to solve a 16x16 unit maze with the start in a corner and goal in the center four squares. Multiple solutions are possible in each maze. [Full rules]( http://micromouseusa.com/wp-content/uploads/2016/04/IEEE-Region-6-Southern-Area-MicroMouse-Competition-Rules.pdf)

This repository contains two major parts -- a maze-solving algorithm testbed (coded in python) meant for a quick development pipeline / visualizations, and the .ino code to run on the micromouse itself. The test folder is a program for pre-competition diagnostics and a general health check. 

Technical details below, but if you want to run the program without downloading anything [check out a hosted colaboratory notebook](www.google.com).

## The algorithm

Given that the micromouse is only allowed 10 minutes to explore, solve, and traverse a maze, the algorithm does not attempt a complete traversal. **After finding the goal, the mouse has to do it again as quickly as possible.** If it did, since the maze is physically explored autonomously, almost all of the cells in the maze would have to be traversed more than once a lot of time would be spent backtracking through known squares. As a result, we decided to have the mouse explore until finding its way to the maze, then to find a different way back to the start (after which it will begin its timed solutions). 

To skip the technical explanation, we use a depth-first traversal to find the goal which tutors towards the goal based on a simple heuristic, a floodfill algorithm to weight each edge's distance from the start and solve by working backwards from the goal.

### The Problem

There are a couple issues with the simple maze solving algorithms from the classic introductory algorithm classes in the context of this problem. First, breadth-first searches won't work because they "teleport" the head of the path, the mouse would spend more time backtracking than moving forward. Put simply, if you were in a hedge maze, it'd be like moving one step to left, then moving back, moving one forward, then moving back again, then moving two left, back, and so on so forth. This problem rules out breadth first searches and the famous best-first A* algorithm, both rely on mouse teleportation for efficiency, which is unfortunately (probably) banned from the competition. 

Recursion is beautiful, but inefficient on embedded systems. Even though the computer on our mouse should have been able to handle some amount of recursion, might as well keep things streamlined to keep everything else on the mouse running smoothly.

Another issue is that maze-solving algorithms lack a level of complexity in physical traversals, namely the assumption that the solver is always standing directly in the center of the cell, and can only travel in cardinal directions. However, moving diagonally is good for simple geometric reasons, and the construction of the maze permits this optimization.

Finally, going straight is better than turning in terms of time. Anyone who's programmed a robot from scratch knows that turning isn't as easy as hard-coded values, and since our mouse notably excludes a gyroscope, turning is a source of error as well as time inefficiency. Thus, the algorithm prefers going straight to turning, which is naturally included in the edge-based diagonal movement logic.

![edge-based solving is better](img\grid.png) 

### General design comments

The mouse in the algorithm testing is designed to be stupid. It sees what the physical mouse would see, what's in front and around it. It's given its own class and its own methods, the maze is kept seperate and is meant to be "searched" unit by unit. 

There are two ways of tracking mouse progression, a string-based print and a drawing. The drawing is only meant for viewing the final solution because well, animations are overkill. A frame-by-frame result is printed into a log file if you want to see the details.

### The results:
![weighted maze](img\weights.png) 
The maze with its weights representing distance from the goal to each point in the maze. The diagonals are ~sqrt(2)/2, 0.707. 
<br>
![maze from the mouse's perspective](img\memory.png) 
The "memory" of the mouse as it traverses the maze. This is its last step in its path towards the goal
<br>
![solved maze](img\result.png)
The path to the goal! Undiscovered walls are in black while purple is the area traversed.



