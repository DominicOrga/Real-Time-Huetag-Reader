#include "opencv2\opencv.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <iostream>
#include <cmath>
#include <conio.h>	
#include "huetagreader.h"
#include "markerholder.h"
#include "line.h"
#include "spritesheet.h"
#include "playablemedia.h"
#include "videofile.h"
#include <thread>

#define OUT

int _blockSizeSlider = 255;
const int _blockSizeSliderMax = 255;
int _cSlider = 10;
const int _cSliderMax = 100;

int _contourMinArea = 5000;
int _contourMaxArea = 125000;

int _trackMinDistDiff = 50;

int _showImage = 0;

std::map<int, orga::playablemedia*> _images;

void onBlockSizeTrackbar(int, void*) {
	if (_blockSizeSlider < 2) {
		_blockSizeSlider = 3;
		return;
	}

	_blockSizeSlider = (_blockSizeSlider % 2 == 0) ? _blockSizeSlider + 1 : _blockSizeSlider;
}

void overlayImage(const cv::Mat &background, const cv::Mat &foreground, cv::Mat &output, cv::Point2i location) {
	background.copyTo(output);

	for (int y = std::max(location.y, 0); y < background.rows; ++y) {
		int fY = y - location.y; 

		if (fY >= foreground.rows)
			break;

		for (int x = std::max(location.x, 0); x < background.cols; ++x) {
			int fX = x - location.x; 

			if (fX >= foreground.cols)
				break;

			double opacity = ((double)foreground.data[fY * foreground.step + fX * foreground.channels() + 3]) / 255.;
			opacity = 1;

			for (int c = 0; opacity > 0 && c < output.channels(); ++c) {
				unsigned char foregroundPx = foreground.data[fY * foreground.step + fX * foreground.channels() + c];
				unsigned char backgroundPx = background.data[y * background.step + x * background.channels() + c];
				output.data[y*output.step + output.channels()*x + c] = backgroundPx * (1. - opacity) + foregroundPx * opacity;
			}
		}
	}
}

void assignMarkers() {
	//_images[14] = new orga::videofile("c:\\Huetag\\stick-figure-fight.mp4");
	_images[14] = new orga::spritesheet(cv::imread("c:\\Huetag\\yurnero.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[1014] = new orga::spritesheet(cv::imread("c:\\Huetag\\mirana.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[10014] = new orga::spritesheet(cv::imread("c:\\Huetag\\traxex.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[1000014] = new orga::spritesheet(cv::imread("c:\\Huetag\\magina.png", CV_LOAD_IMAGE_UNCHANGED));

	orga::spritesheet* phx = new orga::spritesheet;
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-1.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-2.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-3.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-4.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-5.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-6.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-7.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-8.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-9.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-10.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-11.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-12.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-13.png", CV_LOAD_IMAGE_UNCHANGED));
	phx->addSprite(cv::imread("c:\\Huetag\\phx-anim\\phx-frame-14.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[250] = phx;

	orga::spritesheet* es = new orga::spritesheet();
	es->addSprite(cv::imread("c:\\Huetag\\es-anim\\es-frame-1.png", CV_LOAD_IMAGE_UNCHANGED));
	es->addSprite(cv::imread("c:\\Huetag\\es-anim\\es-frame-2.png", CV_LOAD_IMAGE_UNCHANGED));
	es->addSprite(cv::imread("c:\\Huetag\\es-anim\\es-frame-3.png", CV_LOAD_IMAGE_UNCHANGED));
	es->addSprite(cv::imread("c:\\Huetag\\es-anim\\es-frame-4.png", CV_LOAD_IMAGE_UNCHANGED));
	es->addSprite(cv::imread("c:\\Huetag\\es-anim\\es-frame-5.png", CV_LOAD_IMAGE_UNCHANGED));
	es->addSprite(cv::imread("c:\\Huetag\\es-anim\\es-frame-6.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[2500] = es;

	orga::spritesheet* sw = new orga::spritesheet();
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-0.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-1.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-2.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-3.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-4.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-5.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-6.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-7.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-8.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-9.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-11.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-12.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-13.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-14.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-15.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-16.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-17.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-18.png", CV_LOAD_IMAGE_UNCHANGED));
	sw->addSprite(cv::imread("c:\\Huetag\\sw-anim\\sw-frame-19.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[25000] = sw;

	orga::spritesheet* jugg = new orga::spritesheet();
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-1.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-2.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-3.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-4.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-5.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-6.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-7.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-8.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-9.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-10.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-11.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-12.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-13.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-14.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-15.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-16.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-17.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-18.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-19.png", CV_LOAD_IMAGE_UNCHANGED));
	jugg->addSprite(cv::imread("c:\\Huetag\\jugg-anim\\jugg-frame-20.png", CV_LOAD_IMAGE_UNCHANGED));
	_images[250000] = jugg;

}

//cv::VideoCapture _cam(0);
//cv::Mat _frame;

int main() {
	assignMarkers();

	cv::VideoCapture cam(0);

	if (!_cam.isOpened())
		return -1;

	cv::namedWindow("binary", CV_WINDOW_AUTOSIZE);
	cv::createTrackbar("Block Size", "binary", &_blockSizeSlider, _blockSizeSliderMax, onBlockSizeTrackbar);
	cv::createTrackbar("C Size", "binary", &_cSlider, _cSliderMax, 0);
	cv::Mat smoothKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
	cv::Mat grayscale, binary;
	
	cv::namedWindow("main", CV_WINDOW_NORMAL);
	cv::createTrackbar("Show Image", "main", &_showImage, 1, 0);

	cv::Mat blur, main;
	std::vector<orga::markerholder> cachedMarkers;

	cv::Mat frame;
	while (1) {
		if (_kbhit() && _getch() == 's') {
			_showImage = !_showImage;
		}

		//cam >> frame;
		std::thread(_cam.read(frame));
		

		if (!frame.data) {
			continue;
		}

		cv::flip(frame, OUT frame, 1);

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

		// Blur image to reduce noise
		cv::blur(main, OUT blur, cv::Size(3, 3));

		std::vector<orga::markerholder> identifiedMarkers;

		// Idenfify square contours if whether they are Huetags
		max = squareContours.size();
		for (int i = 0; i < max; i++) {
			std::vector<cv::Point*>* squareContour = &squareContours.at(i);

			// Track square contour
			if (!cachedMarkers.empty()) {
				cv::Point* P1 = squareContour->at(0);
				cv::Point* P2 = squareContour->at(1);
				cv::Point* P3 = squareContour->at(2);
				cv::Point* P4 = squareContour->at(3);

				cv::Point center = orga::getIntersection(orga::Line(P1, P3), orga::Line(P2, P4));

				orga::markerholder* nearestCachedContour = NULL;
				float d = 1000000;
				int index = -1;
				// Try to find a near cached contour
				for (int j = 0; j < cachedMarkers.size(); j++) {
					orga::markerholder& temp = cachedMarkers.at(j);

					cv::Point* PP1 = &temp._contour.at(0);
					cv::Point* PP2 = &temp._contour.at(1);
					cv::Point* PP3 = &temp._contour.at(2);
					cv::Point* PP4 = &temp._contour.at(3);

					cv::Point center1 = orga::getIntersection(orga::Line(PP1, PP3), orga::Line(PP2, PP4));

					float d1 = sqrtf(powf(center.x - center1.x, 2) + powf(center.y - center1.y, 2));

					if (d1 < _trackMinDistDiff && d1 < d) {
						nearestCachedContour = &cachedMarkers.at(j);
						d = d1;
						index = j;
					}
				}

				// Check if a close cached marker contour was found.
				if (!nearestCachedContour) {
					goto identify_marker;
				}

				identifiedMarkers.push_back(orga::markerholder(*squareContour, nearestCachedContour->_id));
				//std::cout << "Marker " << i << ": Tracked" << std::endl;
				continue;
			}

			identify_marker:
			
			std::vector<cv::Point*> dataCellPoints;
			orga::extractDataCellPoints(*squareContour, OUT dataCellPoints, 6);
			
			int id = orga::identifyMarkerID(&blur, dataCellPoints);

			if (id != -1) {				
				identifiedMarkers.push_back(orga::markerholder(*squareContour, id));
				//std::cout << "Marker " << i << ": Identified" << std::endl;
			}

			int max1 = dataCellPoints.size();
			for (int j = 0; j < max1; j++)
				delete dataCellPoints.at(j);

			dataCellPoints.clear();
			std::vector<cv::Point*>().swap(dataCellPoints);
		}

		for (int i = 0; i < identifiedMarkers.size(); i++) {
			std::vector<cv::Point>* contour = &identifiedMarkers.at(i)._contour;

			cv::Point& P1 = contour->at(0);
			cv::Point& P2 = contour->at(1);
			cv::Point& P3 = contour->at(2);
			cv::Point& P4 = contour->at(3);
			cv::line(main, P1, P2, cv::Scalar(0, 255, 0), 5);
			cv::line(main, P2, P3, cv::Scalar(0, 255, 0), 5);
			cv::line(main, P3, P4, cv::Scalar(0, 255, 0), 5);
			cv::line(main, P4, P1, cv::Scalar(0, 255, 0), 5);
		}

		for (int i = 0; i < identifiedMarkers.size(); i++) {
			int id = identifiedMarkers.at(i)._id;
			std::vector<cv::Point>* contour = &identifiedMarkers.at(i)._contour;

			cv::Point& P1 = contour->at(0);
			cv::Point& P2 = contour->at(1);
			cv::Point& P3 = contour->at(2);
			cv::Point& P4 = contour->at(3);
			cv::Point center = orga::getIntersection(orga::Line(&P1, &P3), orga::Line(&P2, &P4));

			if (_showImage) {
				if (_images.find(id) != _images.end()) {
					cv::Mat image;
					_images[id]->play(image);

					if (image.data) {
						int w = image.size().width;
						int h = image.size().height;

						std::vector<cv::Point> contour{ P1, P2, P3, P4 };

						float area = cv::contourArea(contour);

						cv::resize(image, image, cv::Size(w * 0.3 * area / 12500, h * 0.3 * area / 12500));
						w = image.size().width;
						h = image.size().height;

						cv::Point imageCenter = cv::Point(center.x - w / 2, center.y - h / 2);
						overlayImage(main, image, main, imageCenter);
					}
				}
			}
			else {
				cv::Size text = cv::getTextSize(std::to_string(id), cv::FONT_HERSHEY_SIMPLEX, 0.75, 1, 0);
				cv::rectangle(main, center - cv::Point(text.width/2, -5), center + cv::Point(text.width / 2, -text.height - 5), CV_RGB(0, 0, 0), CV_FILLED);
				cv::putText(main, std::to_string(id), center - cv::Point(text.width / 2, 0), cv::FONT_HERSHEY_SIMPLEX, 0.75, CV_RGB(255, 255, 255), 1, 8);
			}
		}

		cv::imshow("main", main);

		cachedMarkers.swap(identifiedMarkers);
		identifiedMarkers.clear();

		for (int i = 0; i < max; i++) {
			std::vector<cv::Point*>* temp = &squareContours.at(i);
			int max2 = temp->size();
			for (int j = 0; j < max2; j++)
				delete temp->at(j);
		}	
		squareContours.clear();
		std::vector<std::vector<cv::Point*>>().swap(squareContours);

		//std::cout << std::endl;
		cv::waitKey(30);
	}

	return 0;
}