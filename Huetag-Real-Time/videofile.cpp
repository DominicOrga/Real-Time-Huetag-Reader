#pragma once

#include "videofile.h"
#include <string>

#include <iostream>

namespace orga {
	videofile::videofile(std::string videoPath) {
		_video = cv::VideoCapture(videoPath);
	}

	void videofile::play(cv::Mat& OUT frame) {
		if (!_video.isOpened()) return;

		_video.read(frame);

		if (!frame.data) {
			_video.set(CV_CAP_PROP_POS_FRAMES, 0);
			_video.read(frame)
		}
	}
}