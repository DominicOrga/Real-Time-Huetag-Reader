#pragma once

#ifdef OUT
#define OUT
#endif // 

#include "opencv2\opencv.hpp"
#include "playablemedia.h"

namespace orga {
	class videofile : public playablemedia {
	private:
		cv::VideoCapture _video;

	public:
		videofile(std::string videoPath);
		virtual void play(cv::Mat& OUT frame);
	};
}