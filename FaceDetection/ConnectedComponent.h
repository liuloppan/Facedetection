#pragma once

#include <opencv2/core/core.hpp>
#include <memory>;

using namespace cv;
using namespace std;

class ConnectedComponent
{
private:
	// struct of a Pixel
	struct Pixel {
		int x; //column coord
		int y; //row coord
		Vec3f color; //color of pixel
	};
	// struct of a componant
	struct Component {
		int id; //label or 'id' for a componant
		int size; //number of pixels in the componant
		//vector<shared_ptr<uchar>> pixels; //vector of pointers to pixels
		
	};


	//members of ConnectedComponent
	Mat labelImage; //generated labelImage
	int noComponents; // number of components
	Mat output; //generated components matrice
	//vector<Component> components; //vector containing all components


protected:
	void ComponentLabeling(Mat & Input);


public:
	
	//ConnectedComponent();//construct
	ConnectedComponent(Mat & Input);//construct
	~ConnectedComponent();//destruct





};
