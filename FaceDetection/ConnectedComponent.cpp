#include "ConnectedComponent.h"
//#include <iostream>

//using namespace std;
//
//ConnectedComponent::ConnectedComponent()
//{
//	//members of ConnectedComponent
//	labelImage = (0, 0, CV_32FC1);; //generated labelImage
//	labelImage.setTo(0);
//	output = (0, 0, CV_32FC1);; //generated labelImage
//	output.setTo(0);
//
//	noComponents = 0; // number of components
//	//components = {};
//
//
//}
ConnectedComponent::ConnectedComponent(Mat & Input)
{
	//members of ConnectedComponent
	//labelImage = (Input.rows, Input.cols, CV_32FC1);; //generated labelImage
	//labelImage.setTo(0);
	//output = (Input.rows, Input.cols, CV_32FC1);; //generated labelImage
	//output.setTo(0);

	noComponents = 0; // number of components
	
	

	ComponentLabeling(Input);// ... Contents of your main


}

ConnectedComponent::~ConnectedComponent()
{
}


// this is the main function of the connected component algorithm
void ConnectedComponent::ComponentLabeling(Mat & Input)
{

	try
	{		
	labelImage = (Input.rows, Input.cols, CV_32FC1);; //generated labelImage
	labelImage.setTo(0.0);
	output = (Input.rows, Input.cols, CV_32FC1);; //generated labelImage
	output.setTo(0);


	float label = 1; //start with label 1, 0 is background

		 //first pass
	for (int r = 0; r < Input.rows; r++)
	{

		uchar* accessPtr = Input.ptr<uchar>(r);//used for accessing
		float* labelImagePtr = labelImage.ptr<float>(r); //used for setting

		for (int c = 0; c < labelImage.cols; c++)
		{

			if ((int)accessPtr[c] == 255)
			{ // proceed with algorithm if it's not background

			  //handle cases of neighbors
				if (c == 0 || (labelImagePtr[c - 1]) == label)
				{ //first column or left neighbour
					labelImagePtr[c] = label;
				}
				else
				{ //maybe it is another component, increase label
					label++;
					labelImagePtr[c] = label;
				}
			}
		}
	}

	cout << "I says " << label << "components" << endl;
	//we now know how many labels can maximum exist, we create a mat to store data, keep the count
	Mat compData(1, label, CV_32FC1);
	compData.setTo(0);
	float* compDataPtr = compData.ptr<float>(0);


	//second pass, looping through the res image generated from first pass
	for (int row = 1; row < output.rows; row++) //skip first row
	{
		
		float* rowPtr = output.ptr<float>(row); //current row
		for (int col = 0; col < output.cols; col++)
		{

			if (rowPtr[col] != 0)
			{ // proceed with algorithm if the value is not zero
				
				float* rowPtrPrev = output.ptr<float>(row - 1); //previous row

				if (col != 0) //if not first column, check if there are neighburs
				{
					if (rowPtrPrev[col] != 0)//check if previous row contains a label value
					{
						float copyLabel = rowPtrPrev[col];
						rowPtr[col] = copyLabel;
						compDataPtr[(int)copyLabel]++; //the label is the index in the compData array, increment val
					}
					else if (rowPtrPrev[col - 1] != 0) //if diagonal has label value
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


	}
	catch (cv::Exception & e)
	{
		cerr << e.msg << endl; // output exception message
	}
}
