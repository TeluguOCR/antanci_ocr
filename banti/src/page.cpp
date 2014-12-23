#include <iostream>
#include <sstream>
#include <assert.h>
#include <ctime>
#include "page.h"
#include "math_utils.h"

const float PEAK_THRESHOLD_RATIO = 1 / 10;
const float ZERO_THRESHOLD_RATIO = 1 / 50;

Page::Page(bool asis_mode, int connection4or8) {
	asis_mode_ = asis_mode;
	connection4or8_ = connection4or8;
	pix_orig_ = pix_words_ = pix_lines_ = pix_temp_ = pix_columns_ = pix_100_
			= pix_200_ = pix_400_ = NULL;
	num_cols_ = num_lines_ = 0;
	x_res_ = y_res_ = 0;
	best_harmonic_ = 0;
	base_lines_ = top_lines_ = line_seps_ = vector<int> ();
}

int Page::OpenImage(string name) {
	img_file_name_ = name;
	pix_orig_ = pixRead(name.c_str());
	if (pix_orig_ == NULL) {
		cout << "Pixs Not made, File could not be opened";
		return -1;
	}
	// Print basic pix_ info.
	int width, height, depth;
	pixGetDimensions(pix_orig_, &width, &height, &depth);
	pixGetResolution(pix_orig_, &x_res_, &y_res_);
	cout << "\n\tWidth: " << width << " Height: " << height << " Depth: "
		 << depth << " X Res: " << x_res_ << " Y Res: " << y_res_;

  if (asis_mode_){
	  bool random_rotate = false;
	  if (random_rotate){
		  unsigned rand_from_name = 0;
		  for(unsigned i=0; i < name.length(); ++i)
			  rand_from_name += (unsigned)name[i];
		  srand((unsigned)time(0) + rand_from_name);
		  const float RNG = .2;
		  float ang = (float)rand()/((float)RAND_MAX/(2*RNG)) - RNG;
		  pix_400_  = pixRotate(pix_orig_, 3.1415926535 / 180. * ang,
				  L_ROTATE_AREA_MAP, L_BRING_IN_WHITE, 0, 0);
		  cout << "\n\tRotating by: " << ang;
	  }
	  else
		  pix_400_ = pixClone(pix_orig_);
  }
  else{
	Deskew();
    if (x_res_ > 400) // 400 < resol
    {
        DecreaseResTo(pix_orig_, &pix_400_, 400);
        DecreaseResTo(pix_400_, &pix_200_, 200);
        DecreaseResTo(pix_200_, &pix_100_, 100);
    } else if (x_res_ > 200) // 200 < resol <= 400
    {
        pix_400_ = pixCopy(NULL, pix_orig_);
        DecreaseResTo(pix_400_, &pix_200_, 200);
        DecreaseResTo(pix_200_, &pix_100_, 100);
    } else if (x_res_ > 100) // 100 < resol <= 200
    {
        IncreaseResTo(pix_orig_, &pix_400_, 200);
        pix_200_ = pixCopy(NULL, pix_orig_);
        DecreaseResTo(pix_200_, &pix_100_, 100);
    } else if (x_res_ > 50) // 50 < resol <= 100
    {
        pix_100_ = pixCopy(NULL, pix_orig_);
        IncreaseResTo(pix_100_, &pix_200_, 100);
        IncreaseResTo(pix_200_, &pix_400_, 200);
    } else {
        pixDestroy(&pix_orig_);
        return -1;
    }
  }

	h400_ = pixGetHeight(pix_400_);
	w400_ = pixGetWidth(pix_400_);
	hist_.reserve(h400_);
	morph_hist_.reserve(h400_);
	gaus_hist_.reserve(h400_);
	gaus_morph_hist_.reserve(h400_);
	d_gaus_hist_.reserve(h400_);
	d_gaus_morph_hist_.reserve(h400_);

	return 0;
}

void inline Page::DecreaseResTo(PIX* pix_src, PIX** ppix_target, int targ_res) {
	double res = pixGetXRes(pix_src);
	*ppix_target = pixCopy(NULL, pix_src);
	while (res > targ_res) {
		pix_temp_ = pixReduceRankBinary2(*ppix_target, 2, NULL);
		ResetTempTo(ppix_target);
		res /= 2;
	}
}

void inline Page::IncreaseResTo(PIX* pix_src, PIX** ppix_target, int targ_res) {
	double res = pixGetXRes(pix_src);
	*ppix_target = pixCopy(NULL, pix_src);
	while (res <= targ_res) {
		pix_temp_ = pixExpandBinaryPower2(*ppix_target, 2);
		ResetTempTo(ppix_target);
		res *= 2;
	}
}

Page::~Page() {
	if (pix_orig_)
		pixDestroy(&pix_orig_);
	if (pix_words_)
		pixDestroy(&pix_words_);
	if (pix_lines_)
		pixDestroy(&pix_lines_);
	if (pix_columns_)
		pixDestroy(&pix_columns_);

	if (pix_100_)
		pixDestroy(&pix_100_);
	if (pix_200_)
		pixDestroy(&pix_200_);
	if (pix_400_)
		pixDestroy(&pix_400_);
}

void inline Page::ResetTempTo(PIX** ppix_target) {
	pixDestroy(ppix_target);
	*ppix_target = pix_temp_;
	pix_temp_ = NULL;
}

void Page::FilterNoise() {
	if (!asis_mode_){
		// Median Filter
		pix_temp_ = pixBlockrank(pix_400_, NULL, 1, 1, 0.5);
		ResetTempTo(&pix_400_);
	}
}

void Page::Deskew() {
	// Deskew
    float skew_angle, skew_confidence;
	pix_temp_ = pixFindSkewAndDeskew(pix_orig_, 1, &skew_angle, &skew_confidence);
	ResetTempTo(&pix_orig_);
	cout << "\n\tAngle: " << skew_angle << "\tConfidence : " << skew_confidence;
}

void Page::MorphToWords() {
    BOXA* letter_boxes = pixConnComp(pix_400_, NULL, 8);
    vector<int> wds, hts;
    for(int i=0; i < letter_boxes->n; ++i){
    	wds.push_back(letter_boxes->box[i]->w);
    	hts.push_back(letter_boxes->box[i]->h);
    }
    boxaDestroy(&letter_boxes);
    int median_wd = VecMedian(wds);
    int median_ht = VecMedian(hts);
    cout << "\n\tMedian Letter width: " << median_wd
    	 << " (" << median_wd * 72 / x_res_ << " pts)"
    	 << "\n\tMedian Letter Height: " << median_ht
    	 << " (" << median_ht * 72 / y_res_ << " pts)";
	pix_words_ = pixCloseBrick(NULL, pix_400_,
                                median_wd,
                                median_ht);

	// Find Histogram from Morphed Image (to be used for topline detection)
	Numa* counts = pixCountPixelsByRow(pix_words_, NULL);
	int length = numaGetCount(counts);
	morph_hist_.assign(counts->array, counts->array + length);
	numaDestroy(&counts);
	gaus_morph_hist_ = ConvolveInPlace(morph_hist_, GetGaussianFunc(median_ht>>3, 3));
	DifferentiateInPlace(gaus_morph_hist_, d_gaus_morph_hist_);
}

void Page::MorphToLines() {
	pix_lines_ = pixMorphSequence(pix_200_, "c100.3", 0);
}

void Page::MorphToColumns() {
	pix_columns_ = pixMorphCompSequence(pix_200_, "c5.500", 0);
	num_cols_ = boxaGetCount(pixConnComp(pix_columns_, NULL, 8));
	// Feed a PIXA pointer above if you want the columns stored in pixes
	cout << "\n\tNum columns: " << num_cols_;
}

void Page::MorphAll() {
	//MorphToColumns();
	//MorphToLines();
	MorphToWords();
}

void Page::CalcHist() {
	// Get the values
	Numa* counts = pixCountPixelsByRow(pix_400_, NULL);
	int length = numaGetCount(counts);
	hist_.assign(counts->array, counts->array + length);
	numaDestroy(&counts);

	// Find the best Harmonic
	vector<double> mean_subs;
	mean_subs.reserve(h400_);
	double sum = 0;
	for (int i = 0; i < length; ++i)
		sum += hist_[i];
	sum /= length;
	for (int i = 0; i < length; ++i)
		mean_subs.push_back(hist_[i] - sum);

	vector<double> magfft = MagFFT(mean_subs);
	int max_at, next_2_power = magfft.size(), i = 1;
	double max_val;
	VecMax<double> (magfft.begin() + i, magfft.begin() + (next_2_power / 2),
			max_val, max_at);
	while (max_at == 0){
		cout << "\nCould not detect harmonic.. trying again";
		VecMax<double> (magfft.begin() + (++i), magfft.begin() + (next_2_power / 2),
				max_val, max_at);
	}

	best_harmonic_ = (int) (next_2_power / (max_at + i));
	best_harmonic_amp_ = (int) max_val;
	cout << "\n\tBest Harmonic : " << best_harmonic_ << "\tMax At : " << max_at
		 << "\tPow of 2 : " << next_2_power
		 << "\n\tAmplitude of Best Harmonic : " << best_harmonic_amp_ ;

	// Do a Gaussian blur
	gaus_hist_ = ConvolveInPlace(hist_, GetGaussianFunc(best_harmonic_>>4, 4));
	DifferentiateInPlace(gaus_hist_, d_gaus_hist_);
}

void Page::FindBaseLines() {
	// Use the peaks of the profile to locate each baseline.
	double maxval, gmaxval;
	int maxloc, mintosearch;
	bool inpeak = FALSE;
	int min_dist_in_peak = best_harmonic_ / 2;

	vector<double>& hist = d_gaus_hist_;	// May or may not use morph
	int len = hist.size();
	VecMax(hist, gmaxval, maxloc);

	// Use this to begin locating a new peak
	float peakthresh = gmaxval * PEAK_THRESHOLD_RATIO;
	// Use this to begin a region between peaks
	float zerothresh = gmaxval * ZERO_THRESHOLD_RATIO;

	for (int i = 0; i < len; i++) {
		if (inpeak == FALSE) {
			if (hist[i] > peakthresh) { // transition to in-peak
				inpeak = TRUE;
				maxval = hist[i];
				maxloc = i;
                mintosearch = i + min_dist_in_peak;
                // accept no zeros between i and i+mintosearch
			}
		} else { // inpeak == TRUE; look for max
			if (hist[i] > maxval) {
				maxval = hist[i];
				maxloc = i;
				mintosearch = i + min_dist_in_peak;
			} else if (i > mintosearch && hist[i] <= zerothresh) {
			    // leave peak and save the last baseline found
				inpeak = FALSE;
				base_lines_.push_back(maxloc);
			}
		}
	}

	/* If deriv_gaus_hist[len-1] is max, eg. no descenders, baseline at bottom_ */
	if (inpeak)
		base_lines_.push_back(maxloc);

	num_lines_ = base_lines_.size();
	top_lines_.reserve(num_lines_);
	line_seps_.reserve(num_lines_);
	lines_.reserve(num_lines_);
}

void Page::SeperateLines() {
	int last_found_sep = 0;
	int seperation_at = 0, top_at;
	double dummy_val;
	vector<double>::iterator from, to;

	for (int i = 0; i < num_lines_; ++i) {
		// Find top_ of line
		from = d_gaus_morph_hist_.begin() + last_found_sep + 1;
		to = d_gaus_morph_hist_.begin() + base_lines_[i];
		VecMin<double> (from, to, dummy_val, top_at);
		top_lines_.push_back(last_found_sep + 1 + top_at);

		// Find line separation
		from = gaus_hist_.begin() + base_lines_[i] + 1;
		to = gaus_hist_.begin() + ((i != (num_lines_-1))?base_lines_[i + 1]:h400_);
		VecMin<double> (from, to, dummy_val, seperation_at, (i < num_lines_ - 1));
		// For last line keep the separator as high as possible
		last_found_sep = base_lines_[i] + 1 + seperation_at;
		line_seps_.push_back(last_found_sep);
	}

	int letter_ht = VecMedianDiff<int> (base_lines_, top_lines_);
	if (letter_ht < .2 * best_harmonic_){
		cout << "\n\tMedian Letter Height TOO SMALL? Harmonic could be wrong...\n";
		letter_ht = .2 * best_harmonic_;
	}
	cout << "\n\tMedian Letter Height: " << letter_ht;

	// Populate line info
	lines_.resize(num_lines_);
	for (int i = 0; i < num_lines_; ++i)
		lines_[i].Init((i ? line_seps_[i - 1] + 1 : 0),
                        line_seps_[i],
		                base_lines_[i],
		                pix_400_,
		                i, 0, letter_ht,
		                asis_mode_, connection4or8_);
}

void Page::ProcessLines() {
	for (int iline = 0; iline < num_lines_; ++iline)
		lines_[iline].ProcessLine();
}

void Page::LoadBlobs(vector<Blob>& blobs){
    int num_blobs = 0;
    for (int iline = 0; iline < num_lines_; ++iline)
        num_blobs += lines_[iline].GetNumLetters();
    blobs.resize(num_blobs);
    vector<Blob>::iterator itr = blobs.begin();
    for (int iline = 0; iline < num_lines_; ++iline)
        lines_[iline].LoadBlobs(itr);
    assert(itr == blobs.end());
}

void Page::PrintHistograms(ostream& ost) {
	unsigned int i, len;

	ost << "\nHistogram,GausHistogram,DeriGausHistogram, ";
	len = hist_.size();
	for (i = 0; i < len; i++)
		ost << endl
			<< morph_hist_[i] << ", "
			<< gaus_morph_hist_[i] << ", "
			<< d_gaus_morph_hist_[i];
}

void Page::PrintLinesInfo(ostream& ost) {
	int i;
	ost << "\nToplines(base-top)      : ";
	for (i = 0; i < num_lines_; i++)
		ost << top_lines_[i] << "(" << base_lines_[i] - top_lines_[i] << "), ";

	ost << "\nBaselines(nextbase-base): ";
	for (i = 0; i < num_lines_; i++)
		ost << base_lines_[i] << "(" << base_lines_[i] - (i ? base_lines_[i - 1]
				: 0) << "), ";

	ost << "\nLineSeperations         : ";
	for (i = 0; i < num_lines_; i++)
		ost << line_seps_[i] << ", ";

	ost << "\nWords_in_Line : ";
	for (i = 0; i < num_lines_; i++)
		ost << lines_[i].GetNumWords() << ", ";
}

void Page::DisplayMorphedImages(int reduction) {
	if (pix_400_)
		pixDisplayWriteFormat(pix_400_, reduction, IFF_PNG);
	if (pix_words_)
		pixDisplayWriteFormat(pix_words_, reduction, IFF_PNG);
	if (pix_lines_)
		pixDisplayWriteFormat(pix_lines_, reduction, IFF_PNG);
	if (pix_columns_)
		pixDisplayWriteFormat(pix_columns_, reduction, IFF_PNG);
}

void Page::DisplayLinesImage() {
	PIX* pix_debug = pixConvertTo32(pix_400_);
	for (int i = 0; i < num_lines_; ++i) {
		pixRenderLineArb(pix_debug, 0, top_lines_[i], w400_ - 1, top_lines_[i], 1,
				255, 255, 0);
		pixRenderLineArb(pix_debug, 0, base_lines_[i], w400_ - 1, base_lines_[i],
				1, 0, 255, 0);
		pixRenderLineArb(pix_debug, 0, line_seps_[i], w400_ - 1, line_seps_[i], 1,
				0, 0, 255);
	}
	pixDisplayWriteFormat(pix_debug, 1, IFF_PNG);
	pixDestroy(&pix_debug);
}

void Page::DisplayFromLines() {
	PIX		*pix_debug  = pixConvert1To8(NULL, pix_400_, 255, 0);
	PIXCMAP *cmap 		= pixcmapCreateRandom(8, 1, 1);
	cmap->n = 254;			// Hack to suppress error to draw word boxes
	pixSetColormap(pix_debug, cmap);
	unsigned char index = 1;
	for (int i = 0; i < num_lines_; ++i)
		lines_[i].PrintColorLetters(pix_debug, index);
	pix_debug->colormap->n = 256;
    pixDisplayWriteFormat(pix_debug, 1, IFF_PNG);
    pixDestroy(&pix_debug);
}

void Page::DebugDisplay(ostream &ost, int debug){
    if (debug & 1)
        PrintHistograms(ost);

    if (debug & 2){
        PrintLinesInfo(ost);
		lines_[0].PrintSampleLetter(0);
    }

    if (debug & 4){
        pixDisplayWrite(NULL, -1);
        DisplayMorphedImages();
        DisplayLinesImage();
        DisplayFromLines();
        pixDisplayMultiple("/tmp/junk_write_display*");
    }
}

void Page::PrintBoxInfo(ostream& fout){
    for (int i=0; i < num_lines_; ++i)
        lines_[i].PrintBoxes(fout, pixGetHeight(pix_400_));
}

int Page::Height(){
	return pixGetHeight(pix_400_);
}
