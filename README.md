# Project 5 - Image Viewer

This project build on OpenGL on windows environment's is intended to implement an image viewer software to load and show the PPM  images of type P3 and P6 types. The image viewer software will be able to load PPM images in P3 or P6 format and display the image in a window. Also the software is able to listen to keyboard controls to perform the affine transformations on the image like Rotate in clock and anti-clock directions, zoom-in and zoom out controls, Shear UP, Down, Left and right etc.In this process the program will be able to handle undefined errors with a prefixed error message.

The program needs a PPM input file which should be sent as an command line argument to run the application. 

Steps to run the program :

Steps to run this program - after cloning the repository, use Visual Studio compiler on windows machine. Run visual studio compiler from command line and follow the instructions specified in MakeFile.txt

Step1: Execute the Makefile by typing nmake command - this command will automatically compile the code using the makefile commands.
Step2: Run the software by calling the executable file and the image file as the command line argument for the executable file. Below is the exact command that can be used to run the software.

Run Command :
Imageviewer output.ppm

List of controls for performing the  affine transformations using image viewer:

Rotation:
--> L is turn image left 90 degrees
--> R is turn image right 90 degrees

SCALE
--> "2" is zoom out
--> "1" is zoom in

SHEAR
--> "3" is shear image up
--> "4" is shear image down
--> "5" is shear image left
--> "6" is shear image right

MOVE
--> up arrow is move up on image
--> down arrow is move down on image
--> left arrow is move left on image
--> right arrow is move right on image

QUIT
--> Escape is quit