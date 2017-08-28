#include "opencv2\opencv.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <iostream>
#include <cmath>
#include "huetagreader.h"
#include "markerholder.h"
#include "line.h"

#define OUT

int _blockSizeSlider = 255;
const int _blockSizeSliderMax = 255;
int _cSlider = 10;
const int _cSliderMax = 100;

int _contourMinArea = 5000;
int _contourMaxArea = 125000;

int _trackMinDistDiff = 25;
int _trackMinSizeDiff = 1000;

void onBlockSizeTrackbar(int, void*) {
	if (_blockSizeSlider < 2) {
		_blockSizeSlider = 3;
		return;
	}

	_blockSizeSlider = (_blockSizeSlider % 2 == 0) ? _blockSizeSlider + 1 : _blockSizeSlider;
}

int main() {
	cv::VideoCapture cam(0);

	if (!cam.isOpened())
		return -1;

	cv::namedWindow("binary", CV_WINDOW_NORMAL);
	cv::createTrackbar("Block Size", "binary", &_blockSizeSlider, _blockSizeSliderMax, onBlockSizeTrackbar);
	cv::createTrackbar("C Size", "binary", &_cSlider, _cSliderMax, 0);
	cv::Mat smoothKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::Mat grayscale, binary;
	
	cv::namedWindow("main", CV_WINDOW_NORMAL);

	cv::Mat blur, main;
	std::vector<orga::markerholder> cachedContours;

	cv::Mat frame;
	while (1) {
		cam >> frame;

		cv::cvtColor(frame, OUT grayscale, CV_BGR2GRAY);
		cv::adaptiveThreshold(grayscale, OUT binary, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, _blockSizeSlider, _cSlider);
		cv::erode(binary, OUT binary, smoothKernel);
		cv::erode(binary, OUT binary, smoothKernel);
		cv::dilate(binary, OUT binary, smoothKernel);
		cv::dilate(binary, OUT binary, smoothKernel);
		cv::imshow("binary", binary);

		frame.copyTo(main);
		// Find contours
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(binary, OUT contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		// Find square contours
		std::vector<std::vector<cv::Point*>> squareContours;
		int max = contours.size();
		for (int i = 0; i < max; i++) {
			std::vector<cv::Point> approxSquare;
			cv::approxPolyDP(contours.at(i), OUT approxSquare, cv::arcLength(cv::Mat(contours.at(i)), true) * 0.01f, true);
			float area = cv::contourArea(approxSquare);
			if (approxSquare.size() == 4 && area > _contourMinArea && area < _contourMaxArea) {
				cv::Point P1 = approxSquare.at(0);
				cv::Point P2 = approxSquare.at(1);
				cv::Point P3 = approxSquare.at(2);
				cv::Point P4 = approxSquare.at(3);
				cv::line(main, P1, P2, cv::Scalar(255, 0, 0), 5);
				cv::line(main, P2, P3, cv::Scalar(255, 0, 0), 5);
				cv::line(main, P3, P4, cv::Scalar(255, 0, 0), 5);
				cv::line(main, P4, P1, cv::Scalar(255, 0, 0), 5);

				std::vector<cv::Point*> temp;
				temp.push_back(new cv::Point(P1));
				temp.push_back(new cv::Point(P2));
				temp.push_back(new cv::Point(P3));
				temp.push_back(new cv::Point(P4));
				squareContours.push_back(temp);
			}
		}

		cv::blur(main, OUT blur, cv::Size(3, 3));
		std::vector<orga::markerholder> markerHoldersTemp;
		// Transform squareContours to their data cell coordinates
		max = squareContours.size();
		for (int i = 0; i < max; i++) {
			std::vector<cv::Point*>* squareContour = &squareContours.at(i);

			// Track square contour
			if (!cachedContours.empty()) {
				cv::Point* P1 = squareContour->at(0);
				cv::Point* P2 = squareContour->at(1);
				cv::Point* P3 = squareContour->at(2);
				cv::Point* P4 = squareContour->at(3);

				cv::Point* PP1 = NULL;
				cv::Point* PP2 = NULL;
				cv::Point* PP3 = NULL;
				cv::Point* PP4 = NULL;

				cv::Point center = orga::getIntersection(orga::Line(P1, P3), orga::Line(P2, P4));

				orga::markerholder* nearestCachedContour = NULL;
				float d = 1000000;
				int index = -1;
				// Try to find a near cached contour
				for (int j = 0; j < cachedContours.size(); j++) {
					orga::markerholder& temp = cachedContours.at(j);

					PP1 = &temp._contour.at(0);
					PP2 = &temp._contour.at(1);
					PP3 = &temp._contour.at(2);
					PP4 = &temp._contour.at(3);

					cv::Point center1 = orga::getIntersection(orga::Line(PP1, PP3), orga::Line(PP2, PP4));

					float d1 = sqrtf(powf(center.x - center1.x, 2) + powf(center.y - center1.y, 2));

					//std::cout << d1 << " (" << center.x << "," << center.y << ") & (" << center1.x << "," << center1.y << ") " << std::flush;

					if (d1 < _trackMinDistDiff && d1 < d) {
						nearestCachedContour = &cachedContours.at(j);
						d = d1;
						index = j;
					}
				}

				// Check if a close cached marker contour was found.
				if (!nearestCachedContour) {
					goto identify_marker;
				}

				float perim = 
					sqrtf(powf(P1->x - P2->x, 2) + powf(P1->y- P2->y, 2)) +
					sqrtf(powf(P2->x - P3->x, 2) + powf(P2->y - P3->y, 2)) +
					sqrtf(powf(P3->x - P4->x, 2) + powf(P3->y - P4->y, 2)) +
					sqrtf(powf(P4->x - P1->x, 2) + powf(P4->y - P1->y, 2));

				float perim1 =
					sqrtf(powf(PP1->x - P2->x, 2) + powf(PP1->y - PP2->y, 2)) +
					sqrtf(powf(PP2->x - P3->x, 2) + powf(PP2->y - PP3->y, 2)) +
					sqrtf(powf(PP3->x - P4->x, 2) + powf(PP3->y - PP4->y, 2)) +
					sqrtf(powf(PP4->x - P1->x, 2) + powf(PP4->y - PP1->y, 2));

				// Test if perimeter difference passed threshold
				if (fabs(perim - perim1) > _trackMinSizeDiff) {
					goto identify_marker;
				}

				cv::line(main, *P1, *P2, cv::Scalar(0, 255, 0), 5);
				cv::line(main, *P2, *P3, cv::Scalar(0, 255, 0), 5);
				cv::line(main, *P3, *P4, cv::Scalar(0, 255, 0), 5);
				cv::line(main, *P4, *P1, cv::Scalar(0, 255, 0), 5);
				cv::Size text = cv::getTextSize(std::to_string(nearestCachedContour->_id), cv::FONT_HERSHEY_SIMPLEX, 0.75, 1, 0);
				cv::rectangle(main, center + cv::Point(0, 0), center + cv::Point(text.width, -text.height), CV_RGB(0, 0, 0), CV_FILLED);
				cv::putText(main, std::to_string(nearestCachedContour->_id), center, cv::FONT_HERSHEY_SIMPLEX, 0.75, CV_RGB(255, 255, 255), 1, 8);

				orga::markerholder mh(*squareContour, nearestCachedContour->_id);
				markerHoldersTemp.push_back(mh);

				std::cout << "Marker " << i << ": Tracked" << std::endl;
				continue;
			}

			identify_marker:
			
			std::vector<cv::Point*> dataCellPoints;
			orga::extractDataCellPoints(*squareContour, OUT dataCellPoints, 6);
			int max1 = dataCellPoints.size();

			int id = orga::identifyMarkerID(&blur, dataCellPoints);

			if (id != -1) {
				cv::Point& P1 = *squareContour->at(0);
				cv::Point& P2 = *squareContour->at(1);
				cv::Point& P3 = *squareContour->at(2);
				cv::Point& P4 = *squareContour->at(3);
				cv::line(main, P1, P2, cv::Scalar(0, 255, 0), 5);
				cv::line(main, P2, P3, cv::Scalar(0, 255, 0), 5);
				cv::line(main, P3, P4, cv::Scalar(0, 255, 0), 5);
				cv::line(main, P4, P1, cv::Scalar(0, 255, 0), 5);
				cv::Point center = orga::getIntersection(orga::Line(&P1, &P3), orga::Line(&P2, &P4));
				cv::Size text = cv::getTextSize(std::to_string(id), cv::FONT_HERSHEY_SIMPLEX, 0.75, 1, 0);
				cv::rectangle(main,  center + cv::Point(0, 0), center + cv::Point(text.width, -text.height), CV_RGB(0, 0, 0), CV_FILLED);
				cv::putText(main, std::to_string(id), center, cv::FONT_HERSHEY_SIMPLEX, 0.75, CV_RGB(255, 255, 255), 1, 8);
				
				orga::markerholder mh(*squareContour, id);
				markerHoldersTemp.push_back(mh);

				std::cout << "Marker " << i << "identified" << std::endl;
			}

			for (int j = 0; j < max1; j++)
				delete dataCellPoints.at(j);

			dataCellPoints.clear();
			std::vector<cv::Point*>().swap(dataCellPoints);
		}

		cachedContours.swap(markerHoldersTemp);

		cv::imshow("main", main);

		for (int i = 0; i < max; i++) {
			std::vector<cv::Point*>* temp = &squareContours.at(i);
			int max2 = temp->size();
			for (int j = 0; j < max2; j++)
				delete temp->at(j);
		}	
		squareContours.clear();
		std::vector<std::vector<cv::Point*>>().swap(squareContours);

		std::cout << std::endl;
		cv::waitKey(30);
	}

	return 0;
}