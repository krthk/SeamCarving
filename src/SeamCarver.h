/*
 * SeamCarver.h
 *
 *  Created on: Nov 10, 2013
 *      Author: Karthik Puthraya
 *      E-mail: karthik@puthraya.com
 */

#ifndef SEAMCARVER_H_
#define SEAMCARVER_H_

#include <opencv.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class SeamCarver {
	Mat_<Vec3b> image;
	Mat energy;

	//Compute the full energy matrix by scanning the whole image
	void computeFullEnergy();

	//Compute the energy function only for the pixels affected by seam-removal
	void computeEnergyAfterSeamRemoval(vector<uint> seam);

public:
	SeamCarver(Mat_<Vec3b> im) {
		image = im;
		energy = Mat(image.rows, image.cols, CV_32S, Scalar(195075));
		computeFullEnergy();
	}
//	virtual ~SeamCarver();

	//Show the image
	void showImage() {
		imshow("Image", image);
	}

	//Show the energy matrix as an image
	void showEnergy() {
		imshow("Energy", energy);
	}

	//Return the image
	Mat_<Vec3b> getImage() {
		return image;
	}

	//Return the value of the energy at a particular pixel
	unsigned int getEnergy(unsigned int row, unsigned int col) {
		return energy.at<uint32_t>(row, col);
	}

	//Find the optimal seams
	vector<uint> findVerticalSeam();
	vector<uint> findHorizontalSeam();

	//Remove a given seam and update the image
	void removeVerticalSeam(vector<uint> seam);
	void removeHorizontalSeam(vector<uint> seam);

	//Display a given seam
	void showVerticalSeam(vector<uint> seam);
	void showHorizontalSeam(vector<uint> seam);
};

#endif /* SEAMCARVER_H_ */
