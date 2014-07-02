#include <cassert>
#include <fstream>
#include <iostream>
#include "colorscheme.h"
using namespace std;

#define MAXNUMORDINALCOLORS 6

// These are used for the Spiral color scheme.
// NumTurnsInSpiral determines how many "loops" the spiral makes.
// DistBetweenTurns is a derived value indicating how far apart the loops are.
// PiApproxSixDigit is a 6-digit long approximation of pi.
#define NumTurnsInSpiral 1.0f
#define DistBetweenTurns 1.0f / NumTurnsInSpiral
#define PiApproxSixDigit 3.14159f

vector<rgba> BoysSurfaceColorScheme::boysColors;

rgba ordinalColors[] = {
	rgba(1,0,0),
	rgba(1,1,0),
	rgba(0,1,0),
	rgba(0,1,1),
	rgba(0,0,1),
	rgba(1,0,1)
};

CoolWarmColorScheme::CoolWarmColorScheme(const int numColors) {
	CoolWarmColorScheme::Colors = new float*[numColors];
	for (int i = 0; i < numColors; ++i) {
		CoolWarmColorScheme::Colors[i] = new float[4];
	}

}

/// Ensures that a given value does not exceed specified boundaries.
/** If the value does exceed the boundaries, then one of the boundaries is
 *  returned. If the value is lower than the low boundary, the lower boundary
 *  is returned. If the value is higher than the upper boundary, the upper
 *  boundary is returned.\n
 *  This function works exactly the same way as its int counterpart; the code
 *  is identical. Most likely they both exist to reduce warnings about casting.
 *  @param v the value to check
 *  @param s the lower boundary
 *  @param e the upper boundary
 */
float ColorScheme::clamp(float v, float s, float e){
	if(v > e) { return e; }
	if(v < s) { return s; }
	return v;
}

/// Ensures that a given value does not exceed specified boundaries.
/** If the value does exceed the boundaries, then one of the boundaries is
 *  returned. If the value is lower than the low boundary, the lower boundary
 *  is returned. If the value is higher than the upper boundary, the upper
 *  boundary is returned.\n
 *  This function works exactly the same way as its float counterpart; the code
 *  is identical. Most likely they both exist to reduce warnings about casting.
 *  @param v the value to check
 *  @param s the lower boundary
 *  @param e the upper boundary
 */
int ColorScheme::clamp(int v, int s, int e){
	if(v > e) { return e; }
	if(v < s) { return s; }
	return v;
}

/// Gets the color that is at the specified percent of the way through
/// the color scheme.
/// @param val the percent of the way through the color scheme
rgba CoolWarmColorScheme::GetColorContinuous(const float val){
	// Verifies that value is within the valid range.
	assert(val >= 0 && val <= 1);

	// Calculate hue, chroma, and luma.
	float hue = val * NumTurnsInSpiral * (PiApproxSixDigit * 2.0f);
	float chr = hue * DistBetweenTurns / (PiApproxSixDigit * 2.0f);
	float lum = val;

	// h', here called "hue prime", is defined as "hue / 60 degrees", or
	// "hue / (pi / 3 radians)", or (3 * hue / pi radians).
	float huePrime = fmod(3.0f*hue/PiApproxSixDigit, 2.0f);
	// I have know idea what this number represents, but the relevant paper
	// calls it "x". That's too short for a unique variable name.
	float strangeNumber = DistBetweenTurns * (1 - std::abs(huePrime - 1))/ (2.0f*PiApproxSixDigit);
	// I also don't understand what number is supposed to be, but the paper
	// calls it "m2". That's also too short a name.
	float weirdOffsetM2 = lum - 0.5f*chr;

	// While I needed absolute hue (position within the spiral) for the above
	// calculations, the final step requires relative hue (position within the
	// current loop of the spiral). This line converts it to relative hue.
	hue = fmod(hue, 2*PiApproxSixDigit);

	// The following block of code converts the hue, chroma, luma values to
	// red, green, blue values. It uses the standard process, which can be
	// found easily in Wikipedia.
	/*   */if (0*PiApproxSixDigit/3 <= hue && hue < 1*PiApproxSixDigit/3) {
		return rgba(chr+weirdOffsetM2,strangeNumber+weirdOffsetM2,0+weirdOffsetM2);
	} else if (1*PiApproxSixDigit/3 <= hue && hue < 2*PiApproxSixDigit/3) {
		return rgba(strangeNumber+weirdOffsetM2, chr+weirdOffsetM2, 0+weirdOffsetM2);
	} else if (2*PiApproxSixDigit/3 <= hue && hue < 3*PiApproxSixDigit/3) {
		return rgba(0+weirdOffsetM2, chr+weirdOffsetM2, strangeNumber+weirdOffsetM2);
	} else if (3*PiApproxSixDigit/3 <= hue && hue < 4*PiApproxSixDigit/3) {
		return rgba(0+weirdOffsetM2, strangeNumber+weirdOffsetM2, chr+weirdOffsetM2);
	} else if (4*PiApproxSixDigit/3 <= hue && hue < 5*PiApproxSixDigit/3) {
		return rgba(strangeNumber+weirdOffsetM2, 0+weirdOffsetM2, chr+weirdOffsetM2);
	} else if (5*PiApproxSixDigit/3 <= hue && hue < 6*PiApproxSixDigit/3) {
		return rgba(chr+weirdOffsetM2, 0+weirdOffsetM2, strangeNumber+weirdOffsetM2);
	} else { // This should never be reached. If it is, exit the code.
		printf("There's been a serious error and we need to abort.");
		exit(EXIT_FAILURE); }
}

/// Gets a color for the legend.
/// While this function used to serve a useful purpose, now that the color
/// scheme has been made continuous, it just redirects to the other function.
/// @param value the percent of the way into the color scheme of the color
/// @param numDiscrete the number of colors to show in the legend
rgba CoolWarmColorScheme::GetColorDiscrete(const float value, const int numDiscrete){
	return CoolWarmColorScheme::GetColorContinuous(value);
}

void BoysSurfaceColorScheme::LoadFromFile(const char* filename){
    ifstream input( filename );
	boysColors.clear();
	int n;
    input >> n;
	boysColors.reserve(n);
    for (int t = 0; t < n; ++t) {
		int ts;
        input >> ts;
        float x,y,z;
        input >> x >> y >> z;
		rgba color;
        input >> color.r >> color.g >> color.b ;
        input >> x;
        for (int s = 1; s < ts; ++s){
            input.ignore( 200, '\n');
        }
		boysColors.push_back(color);
    }
}

rgba BoysSurfaceColorScheme::GetColor(int t, int s){
	return boysColors[t];
}

rgba SaturationColorScheme::GetColorContinuous(float value){
	rgba c;
	float v = ColorScheme::clamp(value,0.f,1.f);
	c.fromHSV(10,v,1);
	return c;
}

rgba SaturationColorScheme::GetColorDiscrete(const float value, const int numDiscrete){
	rgba c;
	int t=value*numDiscrete;
	float tt = ColorScheme::clamp((float)t/numDiscrete,0.f,1.f);
	c.fromHSV(10,tt,1);
	return c;
}

rgba FiberColorScheme::GetHighlightColor(){
	return rgba(1,1,0);
}

rgba FiberColorScheme::GetHaloColor(){
	return rgba(0,0,0);
}

rgba OrdinalColorScheme::GetColor(unsigned int value){
	if(value >=  MAXNUMORDINALCOLORS) return ordinalColors[MAXNUMORDINALCOLORS-1];
	return ordinalColors[ColorScheme::clamp(value,0,MAXNUMORDINALCOLORS-1)];
}
