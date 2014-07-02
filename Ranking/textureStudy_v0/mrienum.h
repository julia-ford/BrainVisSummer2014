#ifndef MRIENUM_H
#define MRIENUM_H

#include <string>
using namespace std;
namespace mriEnum{

	typedef enum RetinalChannel_t{
		SATURATION = 1,
		TEXTURE = 2,
		SHAPE = 3,
		SIZE = 4,
		COLOR = 5,
		ORIENTATION = 6,
		POSITION = 7
	} RetinalChannel;

	typedef enum MriTask_t{
		FA = 1,
		BUNDLE_TRACE = 2,
		BUNDLE_SAME = 3,
		BUNDLE_NAME = 4,
		BUNDLE_SIZE = 5,
		LESION = 6
	} MriTask;

	typedef enum Bundle_t{
		CC = 1,
		CST = 2,
		IFO = 3,
		ILF = 4,
		CG = 5
	} Bundle;

	typedef enum Shape_t{
		TUBE = 1,
		SUPERQUADRIC = 2,
		SUPERELLIPSOID = 3,
		RIBBON = 4
	} Shape;

	typedef enum FiberCover_t{
		BUNDLE = 1,
		WHOLE = 2,
		PARTIAL = 3
	} FiberCover;

	string toString(RetinalChannel rc);
	string toString(MriTask task);
	string toString(Bundle bundle);
	string toString(Shape shape);
	string toString(FiberCover cv);

	string RentinalChannelString();
	string MriTaskString();
	string BundleString();
	string ShapeString();
	string FiberCoverString();
}
#endif