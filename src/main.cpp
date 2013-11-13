/*
 * main.cpp
 *
 *  Created on: Nov 11, 2013
 *      Author: Karthik Puthraya
 *      E-mail: karthik@puthraya.com
 */
#include "SeamCarver.h"

int main() {
//	Mat_<Vec3b> image = imread("surfing.png");
//	Mat_<Vec3b> image = imread("pic2.png");
	Mat_<Vec3b> image = imread("stuff.jpg");
	if (!image.data) {
		cout << "Invalid input";
		return -1;
	}

	imshow("Original Image", image);
	SeamCarver s(image);

	for (int i = 0; i < 140; ++i) {
		vector<uint> seam = s.findHorizontalSeam();
//		s.showHorizontalSeam(seam);
		s.removeHorizontalSeam(seam);
	}
	for (int i = 0; i < 300; ++i) {
		vector<uint> seam = s.findVerticalSeam();
//		s.showVerticalSeam(seam);
		s.removeVerticalSeam(seam);
	}
//	imshow("Carved Image", s.getImage());

	imshow("Carved Image", s.getImage());
//	cout << "Seam Length: " << seam.size() << endl;
//	s.showImage();
//	s.showEnergy();
	while (waitKey(20) != 27);

//	for (int i = 0; i < 5; ++i) {
//		for (int j = 0; j < 5; ++j) {
//			cout << s.energy.at<uint32_t>(i,j) << " ";
//		}
//		cout << endl;
//	}
	return 0;
}



