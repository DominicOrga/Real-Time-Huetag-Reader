
#include <vector>
#include "opencv2\opencv.hpp"

namespace orga {
	struct markerholder {
		std::vector<cv::Point*> _contour;
		int _id;
		int _perimeter;

		markerholder(std::vector<cv::Point*>& contour, int id);
		~markerholder();
	};
}