#ifndef mystdexp_h
#define mystdexp_h

#include <vector>
#include <sstream>
#include <string>
#include <cmath>

namespace mystdexp{
	using namespace std;

	template <typename T>
	bool contains(const vector<T> &vec, T value){
		for(unsigned int i = 0;i<vec.size();i++){
			if(vec[i] == value){
				return true;
			}
		}
		return false;
	}

	template <typename T>
	int indexOfFirst(const vector<T> &vec, T value){
		for(unsigned int i = 0;i<vec.size();i++){
			if(vec[i] == value){
				return i;
			}
		}
		return -1;
	}
	
	template <typename T>
	void freePointerVector(vector<T*> &vec){
		for(unsigned int i = 0;i<vec.size();i++){
			// in case same pointers are stored
			// more than once
			if(vec[i] != NULL){
				for(unsigned int j=i+1;j<vec.size();j++){
					if(vec[j] == vec[i]){
						vec[j] = NULL;
					}
				}
				delete vec[i];
				vec[i] = NULL;
			}
		}
	}
	
	template <typename T>
	inline void swap(vector<T>& vec, int a, int b){
		T tmp = vec[b];
		vec[b] = vec[a];
		vec[a] = tmp;
	};

	template <typename T>
	void permute(vector<T> &vec){
		for(unsigned int i = 1;i<vec.size();i++){
			unsigned int toSwap = rand()%(i+1);
			swap(vec,i,toSwap);
		}
	}
	
	template <typename T>
	void permute(vector<T> &vec, unsigned int endIdx, unsigned int startIdx = 0 ){
		for(unsigned int i = startIdx+1;i<endIdx && i< vec.size();i++){
			unsigned int toSwap = rand()%(i-startIdx+1)+startIdx;
			swap(vec,i,toSwap);
		}
	}

	template <typename T>
	void append(vector<T> &vec, const vector<T> &income){
		vec.insert( vec.end(), income.begin(), income.end() );
	}
	
	template <typename T>
	void assign(vector<T> &target, const vector<T> & source, int start, int end){
		target.clear();
		target.reserve(end-start+1);
		for(int i = start;i<=end;i++){
			target.push_back(source[i]);
		}
	}
	

	template <typename T>
	inline int findFirst(vector<T>& vec, const T& value){
		for(unsigned int i = 0;i<vec.size();i++){
			if(vec[i] == value);
			return i;
		}
		return -1;
	};
	
	inline string convertInt(int number)
	{
		stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	};

	inline string convertFloat(float number)
	{
		stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	};
	
	inline string convertChar(char c)
	{
		stringstream ss;//create a stringstream
		ss << c;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	};
}

#endif