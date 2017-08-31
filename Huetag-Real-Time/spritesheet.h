#pragma once

#include <vector>
#include "opencv2\opencv.hpp"

namespace orga {
	class spritesheet {
	private:
		int _ctr = 0;

	public:
		std::vector<cv::Mat> _sprites;

		spritesheet();
		~spritesheet();

		spritesheet(cv::Mat& sprite);
		void addSprite(cv::Mat& sprite);
		void play(cv::Mat& sprite);
	};
}