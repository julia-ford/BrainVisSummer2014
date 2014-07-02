#include "mrienum.h"

using namespace mriEnum;
string mriEnum::toString(RetinalChannel rc){
	switch(rc){
	case SATURATION:
		return "SATURTION";
		break;
	case TEXTURE:
		return "TEXTURE";
		break;
	case SHAPE:
		return "SHAPE";
		break;
	case SIZE:
		return "SIZE";
		break;
	case COLOR:
		return "COLOR";
		break;
	case ORIENTATION:
		return "ORIENTATION";
		break;
	default:
		return "INVALID";
		break;
	}
}

string mriEnum::toString(MriTask task){
	switch(task){
	case FA:
		return "FA";
		break;
	case BUNDLE_TRACE:
		return "TRACE";
		break;
	case BUNDLE_SAME:
		return "SAME";
		break;
	case BUNDLE_NAME:
		return "NAME";
		break;
	case BUNDLE_SIZE:
		return "DENSITY";
		break;
	case LESION:
		return "LESION";
		break;
	default:
		return "INVALID";
		break;
	}
}

string mriEnum::toString(Bundle bundle){
	switch(bundle){
	case CC:
		return "CC";
		break;
	case CST:
		return "CST";
		break;
	case IFO:
		return "IFO";
		break;
	case ILF:
		return "ILF";
		break;
	case CG:
		return "CG";
		break;
	default:
		return "INVALID";
		break;
	}
}

string mriEnum::toString(Shape shape){
	switch(shape){
	case TUBE:
		return "TUBE";
		break;
	case SUPERQUADRIC:
		return "SUPQUAD";
		break;
	case SUPERELLIPSOID:
		return "SUPELIP";
		break;
	case RIBBON:
		return "RIBBON";
		break;
	default:
		return "INVALID";
		break;
	}
}

string mriEnum::toString(FiberCover cv){
	switch(cv){
	case BUNDLE:
		return "BUNDLE";
		break;
	case WHOLE:
		return "WHOLE";
		break;
	case PARTIAL:
		return "PARTIAL";
		break;
	default:
		return "INVALID";
		break;
	}
}
string mriEnum::RentinalChannelString(){
	return "VisCh";
}
string mriEnum::MriTaskString(){
	return "Task";
}
string mriEnum::BundleString(){
	return "Bundle";
}
string mriEnum::ShapeString(){
	return "Shape";
}
string mriEnum::FiberCoverString(){
	return "Cover";
}