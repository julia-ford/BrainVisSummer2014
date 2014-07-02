#ifndef int4_h
#define int4_h

/// This class is the only thing in the file int4.h.
/// The class is used in plenty of other files.
/// It is exactly what it says: it has four ints.
/// It defines no functions besides its constructors.
class int4
{
public:
	int4(void){};
	int4(int _x, int _y, int _z, int _w=1):x(_x),y(_y),z(_z),w(_w){};
	int x,y,z,w;
};

#endif