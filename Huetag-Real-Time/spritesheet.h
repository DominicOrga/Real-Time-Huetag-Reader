#pragma once

#ifndef OUT
#define OUT
#endif // !OUT

#include <vector>
#include "opencv2\opencv.hpp"
#include "playablemedia.h"

namespace orga {
	class spritesheet : public playablemedia {
	private:
		int _ctr = 0;

	public:
		std::vector<cv::Mat*> _sprites;

	public:
		spritesheet();
		spritesheet(cv::Mat& sprite);

		void addSprite(cv::Mat& sprite);
		virtual void play(cv::Mat& OUT frame);
	};
}