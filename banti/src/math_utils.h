#ifndef BANTI_MATH_UTILS_H
#define BANTI_MATH_UTILS_H

#include <vector>
#include <algorithm>
using namespace std;

vector<double> GetGaussianFunc(int sig, int nsigmas);
vector<double> ConvolveInPlace(const vector<double>& f, vector<double> c);
void DifferentiateInPlace(const vector<double>& f,
							    vector<double>& df, vector<double>& ddf);
vector<double> MagFFT(const vector<double>& f);

template<typename T>
void VecMax(typename vector<T>::const_iterator from,
			typename vector<T>::const_iterator till,
			T& max, int& loc, bool prefer_last=true){
	max = *from;
	loc = 0;
	for (typename vector<T>::const_iterator i = from; i != till; ++i){
		if ( (prefer_last && (*i >= max)) ||     	// Preference for last found
			(!prefer_last && (*i >  max))   ){
			max = *i;
			loc = i - from;
		}
	}
}

template<typename T>
void VecMax(vector<T>& f, T& max, int& loc, bool prefer_last=true){
	VecMax(f.begin(), f.end(), max, loc, prefer_last);
}

template<typename T>
void VecMin(typename vector<T>::const_iterator from,
			typename vector<T>::const_iterator till,
			T& min, int& loc, bool prefer_last=true){
	min = *from;
	loc = 0;
	for (typename vector<T>::const_iterator i = from; i != till; ++i){
		if ( (prefer_last && (*i <= min)) ||    	// Preference for last found
			(!prefer_last && (*i <  min))    ){
			min = *i;
			loc = i - from;
		}
	}
}

template<typename T>
void VecMin(const vector<T>& f, T& min, int& loc, bool prefer_last=true){
	VecMin(f.begin(), f.end(), min, loc, prefer_last);
}

template<typename T>
T VecMean(const vector<T>& f){
	int len = f.size();
	T sum = 0;
	for (int i = 0; i < len; ++i)
			sum = sum + f[i];
	return static_cast<T>(sum/len);
}

template<typename T>
T VecMeanDiff(const vector<T>& f, const vector<T>& g){
	int len = f.size();
	T sum = 0;
	for (int i = 0; i < len; ++i)
			sum = sum + (f[i]-g[i]);
	return static_cast<T>(sum/len);
}

template<typename T>
T VecMedian(const vector<T> hin){
	vector<T> h = hin;
	sort(h.begin(), h.end());
	int len = h.size();
	int mid = len>>1;
	return (len%2)?h[mid]:(h[mid]+h[mid-1])/2;
}

template<typename T>
T VecMedianDiff(const vector<T>& f, const vector<T>& g){
	int len = f.size();
	vector<T> h;
	h.reserve(len);
	for (int i = 0; i < len; ++i)
			h.push_back(f[i]-g[i]);
	sort(h.begin(), h.end());
	int mid = len>>1;
	return (len%2)?h[mid]:(h[mid]+h[mid-1])/2;
}

template <typename T>
void IncOrder(T const& values, vector<size_t>& indices) {
    iota(begin(indices), end(indices), static_cast<size_t>(0));
    sort(begin(indices), end(indices),
        [&](size_t a, size_t b) { return values[a] < values[b]; }
    );
}

#endif	// BANTI_MATH_UTILS_H
