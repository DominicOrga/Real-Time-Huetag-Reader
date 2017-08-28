#include "opencv2\opencv.hpp"
#include <vector>
#include <iostream>
#include "huetagreader.h"

#define OUT

int _blockSizeSlider = 255;
const int _blockSizeSliderMax = 255;
int _cSlider = 10;
const int _cSliderMax = 100;

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
	
	cv::namedWindow("main", CV_WINDOW_FULLSCREEN);
	cv::Mat blur, main;

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
		// Find square contours
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(binary, OUT contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		std::vector<std::vector<cv::Point*>*> squareContours;
		int max = contours.size();
		for (int i = 0; i < max; i++) {
			std::vector<cv::Point> approxSquare;
			cv::approxPolyDP(contours.at(i), OUT approxSquare, cv::arcLength(cv::Mat(contours.at(i)), true) * 0.01f, true);
			float area = cv::contourArea(approxSquare);
			if (approxSquare.size() == 4 && area > 5000 && area < 125000) {
				cv::Point P1 = approxSquare.at(0);
				cv::Point P2 = approxSquare.at(1);
				cv::Point P3 = approxSquare.at(2);
				cv::Point P4 = approxSquare.at(3);
				cv::line(main, P1, P2, cv::Scalar(255, 0, 0), 5);
				cv::line(main, P2, P3, cv::Scalar(255, 0, 0), 5);
				cv::line(main, P3, P4, cv::Scalar(255, 0, 0), 5);
				cv::line(main, P4, P1, cv::Scalar(255, 0, 0), 5);

				std::vector<cv::Point*>* temp = new std::vector<cv::Point*>();
				temp->push_back(new cv::Point(P1));
				temp->push_back(new cv::Point(P2));
				temp->push_back(new cv::Point(P3));
				temp->push_back(new cv::Point(P4));
				squareContours.push_back(temp);
			}
		}

		cv::blur(main, OUT blur, cv::Size(3, 3));
		// Transform squareContours to their data cell coordinates
		max = squareContours.size();
		for (int i = 0; i < max; i++) {
			std::vector<cv::Point*>* squareContour = squareContours.at(i);
			
			std::vector<cv::Point*>* dataCellPoints = new std::vector<cv::Point*>();

			orga::extractDataCellPoints(squareContour, OUT dataCellPoints, 6);

			for (int j = 0; j < dataCellPoints->size(); j++) {
				if (j == 0) 
					cv::circle(main, *dataCellPoints->at(j), 2, cv::Scalar(255, 0, 0), 2);
				else if (j == 1) 
					cv::circle(main, *dataCellPoints->at(j), 2, cv::Scalar(255, 0, 255), 2);
				else 
					cv::circle(main, *dataCellPoints->at(j), 1, cv::Scalar(255, 255, 255), 1);

				
			}

			int id = orga::identifyMarkerID(&blur, dataCellPoints);

			std::cout << id << std::endl;

			if (id != -1) {
				cv::Point& P1 = *squareContour->at(0);
				cv::Point& P2 = *squareContour->at(1);
				cv::Point& P3 = *squareContour->at(2);
				cv::Point& P4 = *squareContour->at(3);
				cv::line(main, P1, P2, cv::Scalar(0, 255, 0), 5);
				cv::line(main, P2, P3, cv::Scalar(0, 255, 0), 5);
				cv::line(main, P3, P4, cv::Scalar(0, 255, 0), 5);
				cv::line(main, P4, P1, cv::Scalar(0, 255, 0), 5);

				/*int fontface = cv::FONT_HERSHEY_SIMPLEX;
				int thickness = 1;
				int baseline = 0;

				cv::Size text = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, scale, thickness, &baseline);
				cv::rectangle(image, location + cv::Point(0, baseline), location + cv::Point(text.width, -text.height), CV_RGB(0, 0, 0), CV_FILLED);
				cv::putText(image, label, location, fontface, scale, CV_RGB(255, 255, 255), thickness, 8);
				setLabel(image_temp, cv::String(std::to_string(id)), intersect);*/
			}

			dataCellPoints->clear();
			delete dataCellPoints;
		}

		cv::imshow("main", main);

		squareContours.clear();
		std::vector<std::vector<cv::Point*>*>().swap(squareContours);

		cv::waitKey(30);
	}

	return 0;
}