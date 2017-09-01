#include "spritesheet.h"
#include <iostream>

namespace orga {

	spritesheet::spritesheet() {}

	spritesheet::spritesheet(cv::Mat& sprite) {
		addSprite(sprite);
	}

	void spritesheet::addSprite(cv::Mat& sprite) {
		_sprites.push_back(new cv::Mat(sprite));
	}

	void spritesheet::play(cv::Mat& OUT sprite) {

		_ctr += (_ctr == _sprites.size() - 1) ? -_ctr : 1;

		sprite = *_sprites.at(_ctr);
	}
}