/*
Date: 17/11-2016
Written by: Lovisa Hassler

*****Functionality********
This code segments out facial area in video of a person sitting in front of a computer.
The technique used is filtering out relevant pixels based on color.
Details of the skin color model that is used can be found in the paper
'Face Segmentation Using Skin-Color Map in Videophone Applications' by Chai and Ngan.

*****Limitations********
A face will have a large cluster of skin coloured pixels while the background
may or may not have large clusters of "skin colored" pixels.
Three conditions have to apply for this code to be used successfully.
1. Make sure you have proper lighting for the camera and the room and that the background 
don't have too many objects that can be similar to skin in color.
2. The face have to be the largest skin colored component in the image, this means there
can't be too large clusters of skin colored background noise that can be interpreted as the face
3. Light hair and other 'skin colored" objects can be classified as skin and will not be filtered out
if they are somehow connected to the face.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;


void LargestContourFilter(Mat& anImage, Mat& Result);

int main(int argc, char** argv)
{
	Mat image; // original image

	//video handling
	VideoCapture videoCap; //open camera, 0 is default
	videoCap.open(0);
	videoCap >> image; //read first frame

	//check to see if video worked
	if (!image.data) // Check for invalid input
	{
		cout << "Could not open or find video input" << std::endl;
		return -1;
	}

	//some different image matrices to follow the process
	Mat imYCrCb; //filtered image in YCrCb color space
	Mat skinBinIm(image.rows, image.cols, CV_8UC1); //binary image containging all the potential skinpixels
	Mat contourim;

	// YCrCb range for skin model
	Scalar minYCrCb(0, 133, 77); //min values of skin model in YCrBc color space, originally (0, 133, 77)
	Scalar maxYCrCb(255, 173, 127); // max values of skin model in YCrBc color space, originally (0, 173, 127)

	//Some windows to show input and output
	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	namedWindow("Result", WINDOW_AUTOSIZE); // Create a window for display.

	//***HERE IS WHERE THE MAGIC HAPPENS***//

	while (1) //videoloop
	{
		videoCap >> image; //read next frame from camera
		imshow("Original", image);

		//bgr 2 YCrCb of image
		cvtColor(image, imYCrCb, CV_BGR2YCrCb);

		//Segment to CV_8U type bitmap, white (255) if pixel is within skinmodel, black (0) otherwise
		inRange(imYCrCb, minYCrCb, maxYCrCb, skinBinIm); //result is a 1 channel mat containging type CV_8U

		//to get rid of all unimportant background noise we want the largest component
		//To get rid of them we use the function LargestContourFilter
		//This function will find the largest contours and draw it filled
		LargestContourFilter(skinBinIm, contourim);

		//lastly we cope all relevant skin pixels from the original image
		//by using contourim as a mask
		Mat result; //generate new result matrice everytime
		image.copyTo(result, contourim);

		imshow("Result", result); // display result

		if (waitKey(30) >= 0) break; //break for escape button
									 //waitKey(33); //run forever
	} // end of videoloop

	return 0;
}

//Function accepts only 8-bit 1 channel images
//Generates a 8 bit binary result image that shows the largest contour filled with white
void LargestContourFilter(Mat& anImage, Mat& Result) {

	CV_Assert(anImage.depth() == CV_8U);   // accept only uchar images

	Result = Mat(anImage.rows, anImage.cols, CV_8UC1, Scalar::all(0));
	vector<vector<Point>> contours; //vector of the contours of the binary image
	vector<Vec4i> hierarchy; //hierarchies of the contours

	findContours(anImage, contours, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); //only get external contours

	int largest_area = 0;
	int largest_contour_index = 0;

	for (int i = 0; i< contours.size(); i++) // iterate through each contour to find the largest
	{
		double a = contourArea(contours[i], false);  //  Find the area of contour
		if (a>largest_area)
		{
			largest_area = a;
			largest_contour_index = i;                //Store the index of largest contour
		}
	}
	Scalar color(255, 255, 255);
	drawContours(Result, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy); //draw an fill largest contour
}