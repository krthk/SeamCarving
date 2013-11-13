/*
 * SeamCarver.cpp
 *
 *  Created on: Nov 10, 2013
 *      Author: Karthik Puthraya
 *      E-mail: karthik@puthraya.com
 */

#include "SeamCarver.h"

void SeamCarver::showVerticalSeam(vector<uint> seam) {
	Mat tmp;
	image.copyTo(tmp);
	for (int i = 0; i < tmp.rows; ++i)
		tmp.at<Vec3b>(i, seam[i]) = Vec3b(0, 0, 255);	//Set the color of the seam to Red
	imshow("Seam", tmp);
}

void SeamCarver::showHorizontalSeam(vector<uint> seam) {
	Mat tmp;
	image.copyTo(tmp);
	for (int i = 0; i < tmp.cols; ++i)
		tmp.at<Vec3b>(seam[i], i) = Vec3b(0, 0, 255);	//Set the color of the seam to Red
	imshow("Seam", tmp);
}

void SeamCarver::computeFullEnergy() {
	//Ensure that the size of the energy matrix matches that of the image
	energy.create(image.rows, image.cols, CV_32S);

	//Scan through the image and update the energy values. Ignore boundary pixels.
	for (int i = 1; i < image.rows-1; ++i) {
		uchar* prev = image.ptr<uchar>(i-1);	//Pointer to previous row
		uchar* curr = image.ptr<uchar>(i);		//Pointer to current row
		uchar* next = image.ptr<uchar>(i+1);	//Pointer to next row

		for (int j = 1; j < image.cols-1; ++j) {
			int val = 0;
			//Energy along the x-axis
			val += (prev[3*j]-next[3*j]) * (prev[3*j]-next[3*j]);
			val += (prev[3*j+1]-next[3*j+1]) * (prev[3*j+1]-next[3*j+1]);
			val += (prev[3*j+2]-next[3*j+2]) * (prev[3*j+2]-next[3*j+2]);

			//Energy along the y-axis
			val += (curr[3*j+3]-curr[3*j-3]) * (curr[3*j+3]-curr[3*j-3]);
			val += (curr[3*j+4]-curr[3*j-2]) * (curr[3*j+4]-curr[3*j-2]);
			val += (curr[3*j+5]-curr[3*j-1]) * (curr[3*j+5]-curr[3*j-1]);

			energy.at<uint32_t>(i, j) = val;
		}
	}
}

void SeamCarver::computeEnergyAfterSeamRemoval(vector<uint> seam) {
	Mat tmp = Mat(image.rows, image.cols, CV_32S, Scalar(195075));
	for (unsigned int row = 0; row < (uint)image.rows; ++row) {
		for (unsigned int col = 0; col < (uint)image.cols; ++col) {
			if (col < seam[row]-1)	tmp.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col);
			if (col > seam[row])	tmp.at<uint32_t>(row, col) = energy.at<uint32_t>(row, col+1);
			if (col == seam[row] || col == seam[row]-1) {
				Vec3b l = image.at<Vec3b>(row, col-1);
				Vec3b r = image.at<Vec3b>(row, col+1);
				Vec3b u = image.at<Vec3b>(row-1, col);
				Vec3b d = image.at<Vec3b>(row+1, col);
				int val = (l[0]-r[0])*(l[0]-r[0]) + (l[1]-r[1])*(l[1]-r[1]) + (l[2]-r[2])*(l[2]-r[2]) +
						(u[0]-d[0])*(u[0]-d[0]) + (u[1]-d[1])*(u[1]-d[1]) + (u[2]-d[2])*(u[2]-d[2]);
				tmp.at<uint32_t>(row, col) = val;
			}
		}
	}
	energy = tmp;
}

vector<uint> SeamCarver::findVerticalSeam() {
	vector<uint> seam(image.rows);
	unsigned int distTo[image.rows][image.cols];	//Save the shortest distance from any of the top pixels
	short edgeTo[image.rows][image.cols];			//Which of the the three top pixels, the shortest path came from

	//Initialize the distance and edge matrices
	for (int i = 0; i < image.rows; ++i) {
		for (int j = 0; j < image.cols; ++j) {
			if (i == 0)		distTo[i][j] = 0;
			else			distTo[i][j] = numeric_limits<unsigned int>::max();
			edgeTo[i][j] = 0;
		}
	}

	// Relax the edges in topological order
	for (int row = 0; row < image.rows-1; ++row) {
		for (int col = 0; col < image.cols; ++col) {
			//Check the pixel to the bottom-left
			if (col != 0)
				if (distTo[row+1][col-1] > distTo[row][col] + getEnergy(row+1, col-1)) {
					distTo[row+1][col-1] = distTo[row][col] + getEnergy(row+1, col-1);
					edgeTo[row+1][col-1] = 1;
				}
			//Check the pixel right below
			if (distTo[row+1][col] > distTo[row][col] + getEnergy(row+1, col)) {
				distTo[row+1][col] = distTo[row][col] + getEnergy(row+1, col);
				edgeTo[row+1][col] = 0;
			}
			//Check the pixel to the bottom-right
			if (col != image.cols-1)
				if (distTo[row+1][col+1] > distTo[row][col] + getEnergy(row+1, col+1)) {
					distTo[row+1][col+1] = distTo[row][col] + getEnergy(row+1, col+1);
					edgeTo[row+1][col+1] = -1;
				}
		}
	}

	//Find the bottom of the min-path
	unsigned int min_index = 0, min = distTo[image.rows-1][0];
	for (int i = 1; i < image.cols; ++i)
		if (distTo[image.rows-1][i] < min) min_index = i;

	//Retrace the min-path and update the 'seam' vector
	seam[image.rows-1] = min_index;
	for (int i = image.rows-1; i > 0; --i)
		seam[i-1] = seam[i] + edgeTo[i][seam[i]];

	return seam;
}

void SeamCarver::removeVerticalSeam(vector<uint> seam) {
	//Move all the pixels to the right of the seam, one pixel to the left
	for (int row = 0; row < image.rows; ++row) {
		for (int col = seam[row]; col < image.cols-1; ++col)
			image.at<Vec3b>(row, col) = image.at<Vec3b>(row, col+1);
	}

	//Resize the image to remove the last column
	image = image(Rect(0, 0, image.cols-1, image.rows));

	//Re-compute the energy of the new image
	computeFullEnergy();
//	computeEnergyAfterSeamRemoval(seam);
}

vector<uint> SeamCarver::findHorizontalSeam() {
	vector<uint> seam(image.cols);
	//Transpose the matrices and find the vertical seam
	transpose(image, image);
	transpose(energy, energy);
	seam = findVerticalSeam();

	//Transpose back
	transpose(image, image);
	transpose(energy, energy);
	return seam;
}

void SeamCarver::removeHorizontalSeam(vector<uint> seam) {
	//Transpose the matrices and remove the vertical seam
	transpose(image, image);
	transpose(energy, energy);
	removeVerticalSeam(seam);

	//Transpose back
	transpose(image, image);
	transpose(energy, energy);
}
