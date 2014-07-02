#ifndef rgba_h
#define rgba_h

class rgba{
public:
	rgba(){r = g = b = 0.0; a = 1.0;};
	rgba(float _r,float _g, float _b, float _a = 1.0)
		:r(_r),g(_g),b(_b),a(_a){};

	friend rgba operator*(const rgba &c,const float t){
		return rgba(c.r*t,c.g*t,c.b*t,c.a);
	}

	friend rgba operator*(const float t,const rgba &c){
		return rgba(c.r*t,c.g*t,c.b*t,c.a);
	}

	friend rgba operator*(const rgba &c,const rgba &d){
		return rgba(c.r*d.r,c.g*d.g,c.b*d.b,c.a*d.a);
	}
	
	friend rgba operator+(const rgba &ca, const rgba &cb){
		float rr = ca.r*ca.a+cb.r*cb.a;
		if (rr>1) rr = 1;
		float gg = ca.g*ca.a+cb.g*cb.a;
        if (gg>1) gg = 1;
		float bb = ca.b*ca.a+cb.b*cb.a;
        if (bb>1) bb = 1;
		float aa = 1-(1-ca.a)*(1-cb.a);
		

		return rgba(rr,gg,bb,aa);
	}

	friend rgba operator+(const rgba &ca, const float cb){
		return ca+rgba(cb,cb,cb,1);
	}

	friend rgba operator+( const float cb, const rgba &ca){
		return ca+rgba(cb,cb,cb,1);
	}

	void operator +=(const rgba &ca){
                float rr = ca.r*ca.a+r*a;
                if (rr>1) rr = 1;
                float gg = ca.g*ca.a+g*a;
                if (gg>1) gg = 1;
                float bb = ca.b*ca.a+b*a;
                if (bb>1) bb = 1;
                float aa = 1-(1-ca.a)*(1-a);
		r = rr;
		g = gg;
		b = bb;
		a = aa;
	}
	

	bool isSimiliar(const rgba ca, float diff) const{
		if(r-ca.r > diff || r-ca.r < -diff) return false;
		if(g-ca.g > diff || g-ca.g < -diff) return false;
		if(b-ca.b > diff || b-ca.b < -diff) return false;
		if(a-ca.a > diff || a-ca.a < -diff) return false;
		return true;
	}

	int getMaxIndex() const{
		if (r>g && r>b){
			return 0;
		}
		if (g>r &&g>b){
			return 1;
		}
		return 2;
	}

	int getMinIndex() const{
		if (r<g && r<b){
			return 0;
		}
		if (g<r &&g<b){
			return 1;
		}
		return 2;
	}

	float getMax() const{
		if (r>g && r>b){
			return r;
		}
		if (g>r &&g>b){
			return g;
		}
		return b;
	}

	float getMin() const{
		if (r<g && r<b){
			return r;
		}
		if (g<r &&g<b){
			return g;
		}
		return b;
	}

	float getH() const{
		float c = getMax()-getMin();
		if (c==0){
			return 0;
		}
		int maxIdx = getMaxIndex();
		if (maxIdx == 0){
			return ((int)((g-b)/c))%6 *60.f;
		}
		if (maxIdx == 1){
			return ((b-r)/c+2)*60;
		}
		return ((r-g)/c + 4)*60;
	}

	float getC() const{
		return getMax()-getMin();
	}
	float getI() const{
		return (r+g+b)/3;
	}

	float getV() const{
		return getMax();
	}

	float getL() const{
		return (getMax()+getMin())/2;
	}

	float getSaturation() const{
		return getC()/getV();
	}

	void fromHSV(const float h, const float s, const float v){
		a = 1;
		float c = v*s;
		float hp = h/60;
		float t = hp;
		while(t>=2) t-=2;
		float tmp = t-1;
		float m = v-c;
		tmp=tmp>0?tmp:-tmp;
		float x = c*(1-tmp);
		if (0<=hp && hp<1){
			r = c;
			g = x;
			b = 0;
		}
		else if (1<=hp && hp<2){
			r = x;
			g = c;
			b = 0;
		}
		else if (2<=hp && hp<3){
			r = 0;
			g = c;
			b = x;
		}
		else if (3<=hp && hp<4){
			r = 0;
			g = x;
			b = c;
		}
		else if (4<=hp && hp<5){
			r = x;
			g = 0;
			b = c;
		}
		else if (5<=hp && hp<6){
			r = c;
			g = 0;
			b = x;
		}
		else{
			r = g = b = 0;
		}
		r+=m;
		g+=m;
		b+=m;
	}
	static const rgba black(){return rgba(0,0,0,1);};
	static const rgba white(){return rgba(1,1,1,1);};

public:
	float r;
	float g;
	float b;
	float a;
};

#endif //rgba_h
