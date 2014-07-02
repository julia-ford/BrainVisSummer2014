#include <cmath>
#include "rgba.h"

namespace ColorConverter{

	/// Obviously this is a 2D array of colors.
	/// No idea what it's used for yet.
	int paperRgbs[10][3] ={
		{ 77, 35,103},
		{ 40, 36,103},
		{ 57, 89,143},
		{ 99, 44,165},
		{104,166,141},
		{107,174, 78},
		{131,183, 64},
		{176,203, 84},
		{205,180, 96},
		{213,127, 93},
	};

	/// I think this is a 3D point.
	/** This class has three float variables named x, y, and z.
	 *  Those are its only variables.
	 *  This class defines no functions.
	 */
	class Xyz{
	public:
		float x; /**< x coordinate */
		float y; /**< y coordinate */
		float z; /**< z coordinate */

		/** The default constructor. */
		Xyz(){};

		/// A constructor with xyz values.
		/// @param _x x coordinate
		/// @param _y y coordinate
		/// @param _z z coordinate
		Xyz(float _x,float _y, float _z){
			x = _x;
			y = _y;
			z = _z;
		}
	};

	/// This class, like the "Xyz" class, also appears to be a 3D point.
	/** The three letters in its name, l, a, and b, are the names of this
	 *  class's three float variables. Unlike the Xyz class, it also implements
	 *  a function for doing something with its floats.
	 */
	class Lab{
	public:
		float l; /**< x coordinate? */
		float a; /**< y coordinate? */
		float b; /**< z coordinate? */

		/** Default constructor. */
		Lab(){};

		/// A constructor that specifies the values for the l, a, b floats.
		/// @param _l l value
		/// @param _a a value
		/// @param _b b value
		Lab(float _l,float _a, float _b){
			l = _l;
			a = _a;
			b = _b;
		}

		/// Takes the differences between the floats of two Lab instances,
		/// squares those three differences, and then sums them.
		/// @param other the other Lab instance to compare to
		float distanceSquared(const Lab& other){
			float dl = other.l-l;
			float da = other.a-a;
			float db = other.b-b;
			return dl*dl+da*da+db*db;
		}
	};

	/// Creates a new Xyz instance from specified red, green, and blue
	/// integer values.
	/// @param R red integer
	/// @param G green integer
	/// @param B blue integer
	Xyz rgb2xyz(int R,int G,int B)
	{
			//R from 0 to 255
			//G from 0 to 255
			//B from 0 to 255
		float r = (float)R/255;
		float g = (float)G/255;
		float b = (float)B/255;
 
		if (r > 0.04045){ r = powf((r + 0.055) / 1.055, 2.4); }
		else { r = r / 12.92; }
		if ( g > 0.04045){ g = powf((g + 0.055) / 1.055, 2.4); }
		else { g = g / 12.92; }
		if (b > 0.04045){ b = powf((b + 0.055) / 1.055, 2.4); }
		else {	b = b / 12.92; }
	
		r = r * 100;
		g = g * 100;
		b = b * 100;

		//Observer. = 2°, Illuminant = D65
		Xyz xyz;
		xyz.x = r * 0.4124 + g * 0.3576 + b * 0.1805;
		xyz.y = r * 0.2126 + g * 0.7152 + b * 0.0722;
		xyz.z = r * 0.0193 + g * 0.1192 + b * 0.9505;
		return xyz;
	}
		
	/// Creates a new Lab instance from the specified x, y, and z floats.
	/// @param X x coordinate
	/// @param Y y coordinate
	/// @param Z z coordinate
	Lab xyz2lab(float X, float Y, float Z )
	{
		// TODO: Where did these magic numbers come from?
		const float REF_X =  95.047; // Observer= 2°, Illuminant= D65
		const float REF_Y = 100.000; 
		const float REF_Z = 108.883;

		float x = X / REF_X;   
		float y = Y / REF_Y;  
		float z = Z / REF_Z;  
		
		// TODO: More magic numbers.
		if ( x > 0.008856 ) { x = powf( x , 1.f/3 ); }
		else { x = ( 7.787 * x ) + ( 16.f/116 ); }
		if ( y > 0.008856 ) { y = powf( y , 1.f/3 ); }
		else { y = ( 7.787 * y ) + ( 16.f/116 ); }
		if ( z > 0.008856 ) { z = powf( z , 1.f/3 ); }
		else { z = ( 7.787 * z ) + ( 16.f/116 ); }
 
		// TODO: Even more magic numbers.
		Lab lab;
		lab.l = ( 116 * y ) - 16;
		lab.a = 500 * ( x - y );
		lab.b = 200 * ( y - z );
 
		return lab;
	}
	
	/// Creates a new Xyz instance from l, a, and b floats.
	/// I'm still not sure what lab is.
	Xyz lab2xyz( float l, float a, float b )
	{
			
		const float REF_X = 95.047; // Observer= 2°, Illuminant= D65
		const float REF_Y = 100.000; 
		const float REF_Z = 108.883;
			
		float y = (l + 16) / 116;
		float x = a / 500 + y;
		float z = y - b / 200;
 
		if ( powf( y , 3 ) > 0.008856 ) { y = powf( y , 3 ); }
		else { y = ( y - 16 / 116 ) / 7.787; }
		if ( powf( x , 3 ) > 0.008856 ) { x = powf( x , 3 ); }
		else { x = ( x - 16 / 116 ) / 7.787; }
		if ( powf( z , 3 ) > 0.008856 ) { z = powf( z , 3 ); }
		else { z = ( z - 16 / 116 ) / 7.787; }
 
		Xyz xyz;
		xyz.x = REF_X * x;     
		xyz.y = REF_Y * y; 
		xyz.z = REF_Z * z; 
 
		return xyz;
	}
	
	/// Creates a new rgba instance from xyz coordinates.
	/// The alpha value remains at its default state.
	/// @param X x coordinate
	/// @param Y y coordinate
	/// @param Z z coordinate
	rgba static xyz2rgb(float X, float Y, float Z)
	{
		//X from 0 to  95.047      (Observer = 2°, Illuminant = D65)
		//Y from 0 to 100.000
		//Z from 0 to 108.883
			
		float x = X / 100;        
		float y = Y / 100;        
		float z = Z / 100;        
 
		float r = x * 3.2406 + y * -1.5372 + z * -0.4986;
		float g = x * -0.9689 + y * 1.8758 + z * 0.0415;
		float b = x * 0.0557 + y * -0.2040 + z * 1.0570;
 
		if ( r > 0.0031308 ) { r = 1.055 * powf( r , ( 1 / 2.4 ) ) - 0.055; }
		else { r = 12.92 * r; }
		if ( g > 0.0031308 ) { g = 1.055 * powf( g , ( 1 / 2.4 ) ) - 0.055; }
		else { g = 12.92 * g; }
		if ( b > 0.0031308 ) { b = 1.055 * powf( b , ( 1 / 2.4 ) ) - 0.055; }
		else { b = 12.92 * b; }
			
		rgba rgb;
		rgb.r = r;
		rgb.g = g;
		rgb.b = b;
			
		return rgb;
	}
		
	Lab rgb2lab(const rgba& rgb)
	{
		int R = rgb.r*255;
		int G = rgb.g*255;
		int B = rgb.b*255;
		Xyz xyz= rgb2xyz(R, G, B);
		return xyz2lab(xyz.x, xyz.y, xyz.z);
	}
		
	Lab rgb2lab(int R, int G, int B)
	{
		Xyz xyz= rgb2xyz(R, G, B);
		return xyz2lab(xyz.x, xyz.y, xyz.z);
	}

	rgba lab2rgb(float l, float a, float b)
	{
		Xyz xyz= lab2xyz(l, a, b);
		return xyz2rgb(xyz.x, xyz.y, xyz.z);
	}

	int getIdxByBoysColor(const rgba& color){
		Lab lab = rgb2lab(color);
		static Lab labs[10] = {
			rgb2lab(paperRgbs[0][0],paperRgbs[0][1],paperRgbs[0][2]),
			rgb2lab(paperRgbs[1][0],paperRgbs[1][1],paperRgbs[1][2]),
			rgb2lab(paperRgbs[2][0],paperRgbs[2][1],paperRgbs[2][2]),
			rgb2lab(paperRgbs[3][0],paperRgbs[3][1],paperRgbs[3][2]),
			rgb2lab(paperRgbs[4][0],paperRgbs[4][1],paperRgbs[4][2]),
			rgb2lab(paperRgbs[5][0],paperRgbs[5][1],paperRgbs[5][2]),
			rgb2lab(paperRgbs[6][0],paperRgbs[6][1],paperRgbs[6][2]),
			rgb2lab(paperRgbs[7][0],paperRgbs[7][1],paperRgbs[7][2]),
			rgb2lab(paperRgbs[8][0],paperRgbs[8][1],paperRgbs[8][2]),
			rgb2lab(paperRgbs[9][0],paperRgbs[9][1],paperRgbs[9][2]),
		};
		int minIdx = 0;
		float minDistSq = 10000.f;
		for(int i = 0;i<10;i++){
			Lab lab2 = labs[i];
			float dist = lab.distanceSquared(lab2);
			if(dist<minDistSq){
				minIdx = i;
				minDistSq = dist;
			}
		}
		return minIdx;
	}
}