#pragma once

#include <vector>
#include "opencv2\opencv.hpp"

namespace orga {
	class spritesheet {
		std::vector<cv::Mat> _sprites;
		int _ctr = 0;
		int _spriteCount = 0;

		void addSprite(cv::Mat& sprite);
		void play(cv::Mat& sprite);
	};
}