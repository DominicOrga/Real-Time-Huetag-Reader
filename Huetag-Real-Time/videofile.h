#pragma once

#ifdef OUT
#define OUT
#endif // 

#include "opencv2\opencv.hpp"
#include "playablemedia.h"

namespace orga {
	class videofile : public playablemedia {
	private:
		cv::VideoCapture* _video;
		double _frameCount;
		double _frameCurrent = 0;
		int _frameSkip = 0;

	public:
		videofile(std::string videoPath);
		virtual void play(cv::Mat& OUT frame);
	};
}