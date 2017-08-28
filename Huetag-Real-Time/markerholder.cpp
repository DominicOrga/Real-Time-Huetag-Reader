#include "markerholder.h"

namespace orga {
	markerholder::markerholder(std::vector<cv::Point*>& contour, int id) {
		_id = id;

		_contour.push_back(new cv::Point(*contour.at(0)));
		_contour.push_back(new cv::Point(*contour.at(1)));
		_contour.push_back(new cv::Point(*contour.at(2)));
		_contour.push_back(new cv::Point(*contour.at(3)));
	}

	markerholder::~markerholder() {
		delete _contour.at(0);
		delete _contour.at(1);
		delete _contour.at(2);
		delete _contour.at(3);
	}
}

