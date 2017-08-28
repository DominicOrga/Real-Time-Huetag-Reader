#include "markerholder.h"

namespace orga {
	markerholder::markerholder(std::vector<cv::Point*>& contour, int id) {
		_id = id;

		_contour.push_back(cv::Point(*contour.at(0)));
		_contour.push_back(cv::Point(*contour.at(1)));
		_contour.push_back(cv::Point(*contour.at(2)));
		_contour.push_back(cv::Point(*contour.at(3)));
	}

	markerholder::~markerholder() {
		
	}
}

