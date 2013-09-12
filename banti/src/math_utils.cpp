#include <math.h>
#include "math_utils.h"
#include "gfft.h"


vector<double> GetGaussianFunc(int sig, int nsigmas){
	vector<double> gmf;
	int wing_len = nsigmas*sig ;
	for (int i = -wing_len; i <= wing_len; ++i)
		gmf.push_back(exp( -0.5 * i * i / (sig * sig)) / (sig*sqrt(2*M_PI)));
	return gmf;
}

vector<double> ConvolveInPlace(const vector<double>& f, vector<double> c){
	int length = f.size();
	if (c.size() % 2)	c.push_back(0); 	// Make c of odd length
	int wing_len = (c.size()-1)/2;
	vector<double> ret;
	ret.reserve(length);
	for (int i = 0; i < length; i++ ){
	    double temp = 0;
	    int from = -( (wing_len>i) ? i : wing_len);
	    int to = ((i + wing_len) > (length-1)) ? (length-1-i) : wing_len;
	    for ( int j = from; j <= to; j++ )
	        temp += f[i + j] * c[wing_len + j];
	    ret.push_back(temp);
	}
	return ret;
}

void DifferentiateInPlace(const vector<double>& f,
								vector<double>& df){

	vector<double> differ;
	differ.push_back(1);
	differ.push_back(0);
	differ.push_back(-1);

	df  = ConvolveInPlace(f, differ);
	int n = f.size();
	vector<int> is_zero_crossing(n, 0);

	// Detect Zero Crossing
	for (int i = 1; i < n-1; ++i)
	    if ((df[i-1] > 0.0) != (df[i+1] > 0.0))
	        is_zero_crossing[i] = 1;

	// At Zero Crossing force nearest value to zero
	for (int i = 1; i < n-1; ++i){
	    if (is_zero_crossing[i] && is_zero_crossing[i+1]){
	        if (fabs(df[i]) < fabs(df[i+1]))
	            df[i] = 0;
	        else
	            df[i+1] = 0;
	    }
	}
}

void DifferentiateInPlace(const vector<double>& f,
								vector<double>& df, vector<double>& ddf){

	vector<double> difdiffer;
	difdiffer.push_back(.5);
	difdiffer.push_back(-1);
	difdiffer.push_back(.5);

	ddf = ConvolveInPlace(f, difdiffer);
	DifferentiateInPlace(f, df);
}

using namespace GFFT;
typedef DOUBLE ValueType;
typedef GenerateTransform<1, 16, ValueType, TransformTypeGroup::FullList,
													SIntID<1> > TransformSet;
vector<double> MagFFT(const vector<double>& f){
    int power = 0;
    int length = f.size();
    while (length > (1<<power)) power++;

    typedef DFT TransformType;
    TransformSet gfft;
    TransformSet::ObjectType* fftobj  = gfft.CreateTransformObject(power,
    									   ValueType::ID, TransformType::ID, 1);
    int next2pow = (TransformType::ID == RDFT::ID) ? (1<<(power-1)) : (1<<power);

    // Populate the real part of data (2*i)
    ValueType::ValueType* data = new ValueType::ValueType [2*next2pow]();
    for (int i=0; i < length; ++i)
       data[2*i]   = f[i];

    // Apply FFT in-place
    fftobj->fft(data);

    // Calculate Magnitude - Left half of the transform suffices
    vector<double> magdata;
    magdata.reserve(next2pow);
    for (int i=0; i < next2pow; ++i)
       magdata.push_back(sqrt(pow(data[2*i],2) + pow(data[2*i+1],2)));

    return magdata;
}

