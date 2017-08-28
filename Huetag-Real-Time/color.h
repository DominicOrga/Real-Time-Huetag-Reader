#pragma once

#ifndef OUT
#define OUT
#endif

namespace orga {
	struct RGB {
		float R, G, B;
	};

	struct XYZ {
		float X, Y, Z;
	};

	struct Lab {
		float L, a, b;
	};

	XYZ RGBToXYZ(RGB rgb);
	Lab XYZToLab(XYZ xyz);
	Lab RGBToLab(RGB rgb);
}