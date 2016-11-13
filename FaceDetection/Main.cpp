#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include "opencv2/imgproc.hpp"
//#include <algorithm>

using namespace cv;
using namespace std;

//void FindLargestComponant(Mat&);
void DensityFilter(const Mat& myImage, Mat& Result);
void DensityErodeDilate(Mat& anImage, Mat& Result);

int main(int argc, char** argv)
{

	if (argc != 2)
	{
		cout << " Usage: display_image ImageToLoadAndDisplay" << endl;
		return -1;
	}
	//some different image matrices to follow the 
	Mat image;
	Mat result;
	Mat imYCrCb;
	Mat density;

	// YCrCb range for skin model as recommended by professor 
	Scalar minYCrCb(0, 133, 77); //min values of skin model in YCrBc color space, originally (0, 133, 77)
	Scalar maxYCrCb(255, 173, 127); // max values of skin model in YCrBc color space, originally (0, 173, 127)


	//VideoCapture videoCap; //open camera, 0 is default
	//videoCap.open(0);

	image = imread(argv[1], IMREAD_COLOR); // Read the file in BGR color (BGR is same as RGB but other order)
	//videoCap >> image;
	
	if (!image.data) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}


	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	namedWindow("Result", WINDOW_AUTOSIZE); // Create a window for display.
	namedWindow("FinalImage", WINDOW_AUTOSIZE); // Create a window for display.

	imshow("Original", image); // Show our image inside it.

	//***HERE IS WHERE THE MAGIC HAPPENS***//

	//bgr 2 YCrCb of image
	cvtColor(image, imYCrCb, CV_BGR2YCrCb);

	//Segment to CV_8U type bitmap, white (255) if pixel is within skinmodel, black (0) otherwise
	inRange(imYCrCb, minYCrCb, maxYCrCb, result); //result is a 1 channel mat containging type CV_8U

	Mat dst;
	//Density map handling to get rid of noise
	DensityFilter(result, density); //function that filters iamge into density filter image values
	DensityErodeDilate(density, dst); //function that handles the density values according to neihbouring pixels


	//make connected component map. A face will have a large cluster of skin coloured pixels while the background 
	//may or may not have large clusters of skincolored pixels. Find connected components
	
	Mat ccStats(image.size(), CV_32SC1);
	Mat ccCentroids(image.size(), CV_32SC1);
	Mat labelimage(image.size(), CV_32SC1);

	int nlabels = connectedComponentsWithStats(result, labelimage, ccStats, ccCentroids, 8, CV_32S);
	int componentArea = 0;
	int maxComponentArea = -1;
	int maxLabel = -1;

	//cout << ccStats.rows << ccStats.cols;// << ccStats.channels();

	//get the largest connected component to filter out background noise

	for (int labelcount = 1; labelcount < nlabels; ++labelcount) { // skip 0 which is background
		componentArea = ccStats.at<int>(labelcount, CC_STAT_AREA);

		if (componentArea >= maxComponentArea) 
		{
			maxComponentArea = componentArea;
			maxLabel = labelcount;
			cout << "maxlabel " << maxLabel << "maxcomponentarea "<< maxComponentArea;
		}
	}

	//loop through to find max area labels
	Mat maxComp(image.size(), CV_8UC1);
	for (int r = 0; r < image.rows; ++r) {
		for (int c = 0; c < image.cols; ++c) {
			int label = labelimage.at<int>(r, c);

			if (label == maxLabel) {
				uchar &pixel = maxComp.at<uchar>(r, c);
				pixel = 255.0f;

			}
			else {
				uchar &pixel = maxComp.at<uchar>(r, c);
				pixel = 0.0f;
			}
		}
	}

	//imshow("Result", result); // display result
	Mat finalim(image.size(), CV_8UC3);


	//for (int r = 0; r < image.rows; ++r) {
	//	for (int c = 0; c < image.cols; ++c) {
	//		Vec3b pixel = maxComp.at<Vec3b>(r, c);

	//		if (pixel != Vec3b(0,0,0)) {
	//			Vec3b &pixel = finalim.at<Vec3b>(r, c);
	//			pixel = image.at<Vec3b>(r, c);

	//		}
	//		else {
	//			Vec3b &pixel = finalim.at<Vec3b>(r, c);
	//			pixel = colors[0];
	//		}
	//	}
	//}
	imshow("Result", density); // display result

	imshow("Process", dst); // display result
	imshow("FinalImage", maxComp); // display result

	waitKey(0); // Wait for a keystroke in the window
	return 0;
}


//void filter to density values.
//0 value is defined as highly unlikely being a skin pixel, but depends on surrounding pixels
//0.5 value might be skin pixel, depending on its surroundings
//1.0 is likely to be skin pixel, but can be set to zero if few surrounding pixels are white
void DensityFilter(const Mat& myImage, Mat& Result)
{
	CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images
	Mat anImage;
	myImage.convertTo(anImage, CV_32FC1);
	Result.create(myImage.size(), CV_32FC1);
	//const int nChannels = myImage.channels();

	//first, assign pixelvalues according to neighbouring averages
	for (int j = 2; j < myImage.rows - 2; ++j)
	{
		//create some pointers to the rows for looping
		const float* previous2 = anImage.ptr<float>(j - 2);
		const float* previous = anImage.ptr<float>(j - 1);
		const float* current = anImage.ptr<float>(j);
		const float* next = anImage.ptr<float>(j + 1);
		const float* next2 = anImage.ptr<float>(j + 2);

		float* output = Result.ptr<float>(j);

		for (int i = 2; i < (myImage.cols - 2); ++i)
		{
			float val = (current[i]+ current[i - 1] + current[i + 1] + current[i - 2] + current[i + 2] + 
				previous[i] + previous[i-1] + previous[i + 1] + previous[i - 2] + previous[i + 2] +
				next[i] + next[i - 1] + next[i + 1] + next[i - 2] + next[i + 2] +
				previous2[i] + previous2[i - 1] + previous2[i + 1] + previous2[i - 2] + previous2[i + 2]+
				next2[i] + next2[i - 1] + next2[i + 1] + next2[i - 2] + next2[i + 2] );
			//cout << static_cast<float>(val);
			if (val == 0) //if pixel and all 5x5 neighbours are black
				*output++ = 0.0f;
			else if(val >= (255 * 25)) //if pixel and all 5x5 neighbours are white
				*output++ = 1.0f;
			else //if pixel and its 5x5 neighbours are a mix of black and white
				*output++ = 0.5f;
		}
	}

	//set the edges to black
	Result.row(0).setTo(Scalar(0));
	Result.row(1).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.row(Result.rows - 2).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(1).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0));
	Result.col(Result.cols - 2).setTo(Scalar(0));
}

void DensityErodeDilate(Mat& anImage, Mat& Result) {

	CV_Assert(anImage.depth() == CV_32F);  // accept only float images
	Result.create(anImage.size(), CV_32FC1);

	for (int j = 1; j < anImage.rows - 1; ++j)
	{
		//create some pointers to the rows for looping
		const float* previous = anImage.ptr<float>(j - 1);
		const float* current = anImage.ptr<float>(j);
		const float* next = anImage.ptr<float>(j + 1);

		float* output = Result.ptr<float>(j);



		for (int i = 1; i < (anImage.cols - 1); ++i)
		{
			int skincount = 0;  //reset skincount

			//calculate how many neighbours are white density values aka high probability skin pixels
			for (int r = (j-1); r <= (j+1); ++r) {
				const float* rowPtr = anImage.ptr<float>(r);
				for (int c = (i - 1); c <= (i + 1); ++c){
					if (rowPtr[c] == 1.0f)
					skincount++; //count the number of neighbouring white density values
				}
			}


			if (current[i] == 1.0f) { //set to 0.0f if surrounded by less than 5 other 1.0f values in 3x3 neighbourhood

				if (skincount < 5) //if less than five white neighours
					*output++ = 0.0f;
				else //if pixel and its 3x3 neighbours have 5 or more high density values
					*output++ = 1.0f;

			}
			else {  //if 0.5 or 0.0 set to 1.0f if surrounded by more than two 1.0 (high density)values in 3x3 neighbourhood
				
				if (skincount >= 2) //if two or more neighbouring high density vlues
					*output++ = 1.0f;
				else //if pixel and its 3x3 neighbours have less than two high density values
					*output++ = 0.0f;
			}

		}
	}

}

//int main(int, char**) {
	//
	//	Mat frame;
	//	Mat result;
	//
	//	VideoCapture videoCap; //open camera, 0 is default
	//	videoCap.open(0);
	//	if (!videoCap.isOpened()) { //check if it works
	//		cout << "Could not open or find the video" << std::endl;
	//		return -1;
	//	}
	//
	//	namedWindow("Original video", WINDOW_AUTOSIZE); // Create a window for display.
	//	namedWindow("Result", WINDOW_AUTOSIZE); // Create a window for display.
	//	while (1)
	//	{
	//
	//		videoCap >> frame;
	//
	//		imshow("Original video", frame);
	//		cvtColor(frame, result, COLOR_BGR2GRAY);
	//		//GaussianBlur(result, result, Size(7, 7), 1.5, 1.5);
	//		Canny(result, result, 0, 30, 3);
	//		imshow("Result", result);
	//
	//		if (waitKey(30) >= 0) break; //break for escape button
	//									 //waitKey(33); //run forever
	//	}
	//	//camera deinitialized automatically in videocapture destructor
	//	return 0;
	//};
