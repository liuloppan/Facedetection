#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>


using namespace cv;
using namespace std;

Mat FindLargestComponant(Mat&);

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
	Mat labels;
	//initialize some params
	int IM_HEIGHT;
	int IM_WIDTH;
	// YCrCb range for skin model as recommended by professor 
	Scalar minYCrCb(0, 133, 77); //min values of skin model in YCrBc color space
	Scalar maxYCrCb(255, 173, 127); // max values of skin model in YCrBc color space
	//contours

	//VideoCapture videoCap; //open camera, 0 is default
	//videoCap.open(0);

	image = imread(argv[1], IMREAD_COLOR); // Read the file in BGR color (BGR is same as RGB but other order)
	//videoCap >> image;

	if (!image.data) // Check for invalid input
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	IM_HEIGHT = image.size[0];
	IM_WIDTH = image.size[1];

	namedWindow("Original", WINDOW_AUTOSIZE); // Create a window for display.
	namedWindow("Result", WINDOW_AUTOSIZE); // Create a window for display.

	imshow("Original", image); // Show our image inside it.

	//***HERE IS WHERE THE MAGIC HAPPENS***//

	//bgr 2 YCrCb of image
	cvtColor(image, imYCrCb, CV_BGR2YCrCb);

	//Segment to CV_8U type bitmap, white (255) if pixel is within skinmodel, black (0) otherwise
	inRange(imYCrCb, minYCrCb, maxYCrCb, result);

	//make connected component map. A face will have a large cluster of skin coloured pixels while the background 
	//may or may not have large clusters of skincolored pixels. Find connected components

	//Mat labelImage(img.size(), CV_32S);
	//imshow("Result", result);
	//Mat test = FindLargestComponant(result);
	Mat labelImage(image.size(), CV_32S);
	int nLabels = connectedComponents(result, labelImage, 8);
	std::vector<Vec3b> colors(nLabels);
	colors[0] = Vec3b(0, 0, 0);//background
	
	for (int label = 1; label < nLabels; ++label) {
		colors[label] = Vec3b((rand() & 255), (rand() & 255), (rand() & 255));
	}
	Mat dst(image.size(), CV_8UC3);
	for (int r = 0; r < dst.rows; ++r) {
		for (int c = 0; c < dst.cols; ++c) {
			int label = labelImage.at<int>(r, c);
			Vec3b &pixel = dst.at<Vec3b>(r, c);
			pixel = colors[label];
		}
	}

	//cout << result.rows << result.cols;
	//perform erosion on background to get less noise
	//erode(result, result, Mat(), Point(-1, -1), 3, 0, morphologyDefaultBorderValue());

	//perform dilation in the facial area to get less noise 
	//dilate(result, result, Mat(), Point(-1,-1), 3,0, morphologyDefaultBorderValue());




	//cout << labelImage;
	//imshow("Result", result); // display result

	imshow("Result", dst); // display result

	waitKey(0); // Wait for a keystroke in the window
	return 0;
}


Mat FindLargestComponant(Mat &inputIm){

	Mat res(inputIm.rows, inputIm.cols, CV_32FC1); //initialize generated picture with zeros
	res.setTo(0);
	Mat output(inputIm.rows, inputIm.cols, CV_32FC1); //initialize generated picture with zeros
	output.setTo(0);

	//int pixCount = 0;
	//Vec3i pixData; // data of a pixel, stores number of connected pixels as int and its image coordinates (row, col)
	float label = 1;
	
	//vector<Vec3i> compData; //each slot in compData is the label of a componant. Each slot stores a pixData

	//first pass
	for (int r = 0; r < inputIm.rows; r++)
	{
		
		uchar* accessPtr = inputIm.ptr<uchar>(r);//used for accessing
		float* resPtr = res.ptr<float>(r); //used for setting

		for (int c = 0; c < inputIm.cols; c++)
		{

			if ((int)accessPtr[c] == 255)
			{ // proceed with algorithm if it's not background

				//handle cases of neighbors
				if (c == 0 || (resPtr[c-1]) == label)
				{ //first column or left neighbour
					resPtr[c] = label;
				}
				else
				{ //maybe it is another component, increase label
					label++;
					resPtr[c] = label;
				}
			}
		  }
	}

	////we now know how many labels exist, we create a mat to store data, keep the count
	Mat compData(1, label, CV_32FC1);
	compData.setTo(0);
	float* compDataPtr = compData.ptr<float>(0);
	//vector<Vec3b> colors(label);
	//colors[0] = Vec3b(0, 0, 0);//background

	//second pass, looping through the res image generated from first pass
	for (int row = 1; row < res.rows; row++) //skip first row
	{
		float* rowPtr = res.ptr<float>(row); //current row
		for (int col = 0; col < res.cols; col++)
		{

			if (rowPtr[col] != 0)
			{ // proceed with algorithm if the value is not zero

				float* rowPtrPrev = res.ptr<float>(row-1); //previous row

				if (col != 0) //if not first column, check if there are neighburs
				{ 
					if (rowPtrPrev[col] != 0)//check if previous row contains a label value
					{ 
						float copyLabel = rowPtrPrev[col];
						rowPtr[col] = copyLabel;
						compDataPtr[(int)copyLabel]++; //the label is the index in the compData array, increment val
					}
					else if(rowPtrPrev[col - 1] != 0) //if diagonal has label value
					{ 
						float copyLabel = rowPtrPrev[col - 1];
						rowPtr[col] = copyLabel;
						compDataPtr[(int)copyLabel]++; //the label is the index in the compData array, increment val
					}
					else if (rowPtr[col - 1] != 0) //left pixel is not zero
					{
						float copyLabel = rowPtr[col - 1];
						rowPtr[col] = copyLabel;
						compDataPtr[(int)copyLabel]++; //the label is the index in the compData array, increment val
					}
					else {}

				}
				else 
				{
					if (rowPtrPrev[col] != 0) //if it is first col, check above
					{
						float copyLabel = rowPtrPrev[col];
						rowPtr[col] = copyLabel;
						compDataPtr[(int)copyLabel]++;
					}
				}

			}

		}
	}
	//now we want to find the 3 largest componants

	int pix = 0; //number of pixels in a component
	int maxLabel = 0; //label of the largest componant


	for (int i = 0; i < compData.cols; i++)
	{
		if (compDataPtr[i] != 0)
		{ 

			if (compDataPtr[i] > pix)
			{
				cout << "i =" << i << " pixels " << compDataPtr[i] << endl;
				pix = compDataPtr[i];
				maxLabel = i;
			}
		}
	}
	cout << "maxlabel = " << maxLabel << " maxpix" << pix << endl;
	//make the new black and white image
	for (int r = 0; r < inputIm.rows; r++)
	{

		uchar* accessPtr = inputIm.ptr<uchar>(r);//used for accessing
		float* outputPtr = output.ptr<float>(r); //used for setting

		for (int c = 0; c < inputIm.cols; c++)
		{
			if ((int)outputPtr[c] != 0 ) //== maxLabel)
			{
				accessPtr[c] = (uchar)255;
			}
			else
			{
				accessPtr[c] = (uchar)0;
			}
		}
	}

	return output;

};





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
