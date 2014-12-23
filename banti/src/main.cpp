#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "page.h"
#include "classifier.h"

using namespace std;

void PrintBestMatches(vector<Blob>& vb, ostream& out=cout,
					  int n=8, bool print_dists=true){
	out.setf(ios::fixed);
    for (size_t i=0; i<vb.size(); ++i){
    	out << vb[i].line_id_ << " " << vb[i].word_id_ << " ";
        for (int j=0; j<n; ++j){
        	size_t jthbest = vb[i].best_matches_[j];
    		out << char_codes[jthbest] << " "
    		   << long(vb[i].sq_dist_to_means_[jthbest])<< " ";
        }
		out << "\n";
    }
}

void PrintMostLikelyText(vector<Blob>& vb, ostream& out){
	int word = 0, line = 0;
	for (size_t i=0; i<vb.size(); ++i){
		if (vb[i].word_id_ > word)
			out << " ";
		if (vb[i].line_id_ > line)
			out << "\n";
		word = vb[i].word_id_;
		line = vb[i].line_id_;
		out << char_codes[vb[i].best_matches_[0]];
	}
}

inline string ChangeFileExtension(const string& name, const string& ext){
	return name.substr(0, name.find_last_of('.')) + ext;
}

int main(int    argc,     char **argv){
    if (argc < 2){
        cout << "Usage:\nbanti filename.tif [debug_flag=0] [mode=1]"
             << "\n debug_flag(0-7)"
             << "\n    0 Print/Show nothing (default)"
             << "\n   +1 Print Histograms (Use even number to avoid this)"
             << "\n   +2 Print Line Separation, Baselines, Toplines etc."
             << "\n   +4 Show Cool Images"
             << "\n mode"
             << "\n    0 : No Box file, No Classification, No AsIs"
             << "\n   +1 : Write Box file in normal style (default)"
             << "\n   +3 : Write Box file in Tesseract Style"
             << "\n   +4 : Run Classifier"
             << "\n   +8 : asis_mode (Do not adjust for noise, tilt etc.)"
             << "\n"
             ;
        return -1;
    }

    string filename(argv[1]);
    int debug_flags = 0;
    if (argc > 2)
    	debug_flags = atoi(argv[2]);

    int mode = 1;
    if (argc > 3)
    	mode = atoi(argv[3]);

    bool write_box = (bool)(mode & 1);
    bool tesseract_style = (bool)(mode & 2);
    bool run_classifier = (bool)(mode & 4);
    bool asis_mode = (bool)(mode & 8);

    Page mypage(asis_mode);

    cout << "Processing "<< filename
         << "\n\tMode : " << mode
		 << ", Write Box: " << write_box
		 << ", Tesseract Style: " << tesseract_style
		 << ", Run Classifier: " << run_classifier
		 << ", As-Is Mode: " << asis_mode << "\n";

    if (mypage.OpenImage(filename) != 0)
    	return -1;
	mypage.FilterNoise();
    mypage.MorphAll();
    mypage.CalcHist();
    mypage.FindBaseLines();
    mypage.SeperateLines();
    mypage.ProcessLines();

    vector<Blob> blobs;
    if (run_classifier || write_box)
    	mypage.LoadBlobs(blobs);

	mypage.DebugDisplay(cout, debug_flags);

	if(run_classifier){
	    string exe(argv[0]);
	    Classifier lda(exe.substr(0, exe.find_last_of('/')));
		lda.PopulateFeatures(blobs);

		ofstream ftext;
		ftext.open(ChangeFileExtension(filename, ".txt").c_str());
		PrintMostLikelyText(blobs, ftext);
		ftext.close();

		ofstream fmatches;
		fmatches.open(ChangeFileExtension(filename, ".match").c_str());
		PrintBestMatches(blobs, fmatches);
		fmatches.close();

		cout << "\nWrote output to "
			 << ChangeFileExtension(filename, ".txt")
			 << "\nWrote top N matches (with distances) to "
			 << ChangeFileExtension(filename, ".match");
	}

    if (write_box){
        ofstream fbox;
        fbox.open(ChangeFileExtension(filename, ".box").c_str());
        for (unsigned int i=0; i<blobs.size(); i++)
        	blobs[i].PrintBoxInfo(fbox, mypage.Height(), tesseract_style);
        fbox.close();
        cout << "\nWrote box file info to "
        	 << ChangeFileExtension(filename, ".box");
        if (tesseract_style)
        	cout << " (in Tesseract style)";
    }

    cout << endl;
    return 0;
}

