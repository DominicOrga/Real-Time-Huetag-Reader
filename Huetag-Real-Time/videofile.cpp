#pragma once

#include "videofile.h"
#include <string>

#include <iostream>

namespace orga {
	videofile::videofile(std::string videoPath) {
		_video = new cv::VideoCapture(videoPath);
		_frameCount = _video->get(CV_CAP_PROP_FRAME_COUNT);
	}

	void videofile::play(cv::Mat& OUT frame) {
		if (!_video) return;

		//_frameCurrent += (_frameCurrent >= _frameCount - 1 - _frameSkip) ? -_frameCurrent : 1 + _frameSkip;

		//_video->set(CV_CAP_PROP_POS_FRAMES, _frameCurrent);
		_video->read(frame);
	}
}