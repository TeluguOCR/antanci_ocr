#ifndef BANTI_PAGE_H
#define BANTI_PAGE_H

#include <vector>
#include "line.h"
using namespace std;

class Page{
protected:
    string img_file_name_;

    int x_res_, y_res_;
    int w400_, h400_;
    int best_harmonic_, best_harmonic_amp_;
    int num_cols_;							// TODO: Abstract Columns
    int num_lines_;

    PIX *pix_orig_;
    PIX *pix_100_;						// Sizes ( 50, 100]
    PIX *pix_200_;						// Sizes (100, 200]
    PIX *pix_400_;						// Sizes (200, 400]

    PIX *pix_temp_;       // Just to avoid too many declarations in functions
    PIX *pix_words_;
    PIX *pix_lines_;
    PIX *pix_columns_;

    vector<double> hist_;
    vector<double> gaus_hist_, d_gaus_hist_, dd_gaus_hist_;
    vector<int> base_lines_;
    vector<int> top_lines_;
    vector<int> line_seps_;
    vector<Line> lines_;

    void MorphToLines();
    void MorphToWords();
    void MorphToColumns();
    void inline ResetTempTo(PIX** ppix_target);
    void inline DecreaseResTo(PIX* pix_src, PIX** ppix_target, int targ_res);
    void inline IncreaseResTo(PIX* pix_src, PIX** ppix_target, int targ_res);

public:
    Page();
    ~Page();

    int OpenImage(string name);
    void FilterNoise();
    void Deskew();
    void CalcHist();
    void FindBaseLines();
    void SeperateLines();
    void MorphAll();

    void ProcessLines();
    void LoadBlobs(vector<Blob>& blobs);

    void PrintHistograms(ostream& ost);
    void PrintLinesInfo(ostream& ost);
    void DisplayMorphedImages(int reduction=1);
    void DebugDisplay(ostream &ost, int debug);
    void DisplayLinesImage();
    void DisplayFromLines();
    void PrintBoxInfo(ostream& fout);
    int Height();
};

#endif // BANTI_PAGE_H
