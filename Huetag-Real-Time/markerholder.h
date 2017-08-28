
#include <vector>
#include "opencv2\opencv.hpp"

namespace orga {
	class markerholder {
	public:
		std::vector<cv::Point> _contour;
		int _id;
	public:
		markerholder(std::vector<cv::Point*>& contour, int id);
		~markerholder();
	};
}