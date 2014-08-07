#ifndef MRIENUM_H
#define MRIENUM_H

#include <string>
using namespace std;
namespace mriEnum{

	/// This enum defines the encodings used to represent the FA values of the
	/// bundles. There are seven encodings listed.
	typedef enum RetinalChannel_t{
		SATURATION = 1,
		TEXTURE = 2,
		SHAPE = 3,
		SIZE = 4,
		COLOR = 5,
		ORIENTATION = 6,
		POSITION = 7
	} RetinalChannel;

	/// This enum defines the types of tasks that participants will be asked
	/// to perform. There are six tasks listed.
	typedef enum MriTask_t{
		FA = 1,
		BUNDLE_TRACE = 2,
		BUNDLE_SAME = 3,
		BUNDLE_NAME = 4,
		BUNDLE_SIZE = 5,
		LESION = 6
	} MriTask;

	/// This enum defines the type of data to be used for a given trial.
	/// Depending on the value, different parts of the data will be loaded.
	typedef enum Bundle_t{
		// corpus callosum
		CC = 1,
		// cortospinal tracts
		CST = 2,
		// inferior frontal occipital fasciculus
		IFO = 3,
		// inferior longitudinal fasciculus
		ILF = 4,
		// cingulum
		// not in use because it is too small
		CG = 5
	} Bundle;

	/// This enum defines the shapes that a fiber can be drawn as.
	/// There are four shapes listed.
	typedef enum Shape_t{
		TUBE = 1,
		SUPERQUADRIC = 2,
		SUPERELLIPSOID = 3,
		RIBBON = 4
	} Shape;

	/// This enum defines the amount of fibers that will be drawn. For example,
	/// WHOLE means that all the fibers in the data will be drawn, and BUNDLE
	/// means that only a single bundle will be drawn.
	/// There are three types of fiber cover listed.
	typedef enum FiberCover_t{
		BUNDLE = 1,
		WHOLE = 2,
		PARTIAL = 3
	} FiberCover;

	// toString functions for each type of enum

	string toString(RetinalChannel rc);
	string toString(MriTask task);
	string toString(Bundle bundle);
	string toString(Shape shape);
	string toString(FiberCover cv);

	// constructors for each type of enum

	string RentinalChannelString();
	string MriTaskString();
	string BundleString();
	string ShapeString();
	string FiberCoverString();
}
#endif