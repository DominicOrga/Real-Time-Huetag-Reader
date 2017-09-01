#pragma once

#ifndef OUT
#define OUT
#endif // !OUT

#include "opencv2\opencv.hpp"

namespace orga {
	class playablemedia {
	public:
		virtual void play(cv::Mat& OUT frame) {};
	};
}
