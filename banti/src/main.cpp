#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include "page.h"
#include "classifier.h"

using namespace std;
bool training_mode = false;

/*
void PrintBestMatches(vector<Blob>& vb, ostream& out=cout,
					  int n=8, bool print_dists=true){
	out.setf(ios::fixed);
    for (size_t i=0; i<vb.size(); ++i){
    	out << endl << (i+1) << " = ";
        for (int j=0; j<n; ++j){
        	size_t jthbest = vb[i].best_matches_[j];
    		out << "\t" << char_codes[jthbest];
    		if(print_dists)
    		out << "\t(" << setw(3) << 1+jthbest << ") - "
    		   << std::setprecision(3) << setw(6)
    		   << vb[i].sq_dist_to_means_[jthbest] / 1000000
    		   << "M\n";
        }
    }
}*/
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
        cout << "Usage:\nbanti filename.tif [debug_flag=0] [training_mode=0]"
             << "\n debug_flag(0-7)"
             << "\n   0 Do nothing"
             << "\n   1 Print Histograms (Use even number to avoid this)"
             << "\n   2 Print Line Separation, Baselines, Toplines etc."
             << "\n   4 Show Cool Images"
             << "\n  You can also give sums of the above numbers"
             << "\n training_mode"
             << "\n   Unspecified: filename.box is NOT written"
             << "\n   mode = 1   : will not run classifier"
             << "\n   mode = 0   : will write box file in Tesseract style"
             << "\n"
             ;
        return -1;
    }
    bool write_box = (argc > 3);
    if (argc > 3)
    	if (atoi(argv[3]) == 1)
    		training_mode = true;

    string filename(argv[1]);
    Page mypage;

    cout << "Processing "<< filename;
    if (mypage.OpenImage(filename) != 0)
    	return -1;
    if (!training_mode)
    	mypage.FilterNoise();
    mypage.MorphAll();
    mypage.CalcHist();
    mypage.FindBaseLines();
    mypage.SeperateLines();
    mypage.ProcessLines();

    vector<Blob> blobs;
    mypage.LoadBlobs(blobs);

    if (argc > 2)
        mypage.DebugDisplay(cout, atoi(argv[2]));

	if(!training_mode){
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
        	blobs[i].PrintBoxInfo(fbox, mypage.Height());
        fbox.close();
        cout << "\nWrote box file info to "
        	 << ChangeFileExtension(filename, ".box");
        if (!training_mode)
        	cout << " (in Tesseract style)";
    }

    cout << endl;
    return 0;
}

