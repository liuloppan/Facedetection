# Facedetection

Language: C++ <br />
API: OpenCV 3.1 <br />

Date: 17/11-2016 <br />
Written by: Lovisa Hassler <br />
<br />
-----------Functionality--------- <br />
This code segments out facial area in video of a person sitting in front of a computer.
The technique used is filtering out relevant pixels based on color.
Details of the skin color model that is used can be found in the paper
'Face Segmentation Using Skin-Color Map in Videophone Applications' by Chai and Ngan.


![Alt text](master/data/example2.png?raw=true "Result")

----------Limitations----------- <br />
A face will have a large cluster of skin coloured pixels while the background
may or may not have large clusters of "skin colored" pixels.
Three conditions have to apply for this code to be used successfully.
<br />
1. Make sure you have proper lighting for the camera and the room and that the background 
don't have too many objects that can be similar to skin in color.
<br />2. The face have to be the largest skin colored component in the image, this means there
can't be too large clusters of skin colored background noise that can be interpreted as the face
<br />3. Light hair and other 'skin colored" objects can be classified as skin and will not be filtered out
if they are somehow connected to the face.
