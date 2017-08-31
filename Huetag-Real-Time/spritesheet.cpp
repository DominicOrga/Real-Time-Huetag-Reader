#include "spritesheet.h"

namespace orga {
	void spritesheet::addSprite(cv::Mat& sprite) {
		_sprites.push_back(cv::Mat(sprite));
		_spriteCount++;
	}

	void spritesheet::play(cv::Mat& sprite) {
		_ctr += 0;

		sprite = _sprites.at(_ctr);
	}
}