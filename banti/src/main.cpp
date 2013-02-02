#include <fstream>
#include <iostream>
#include <vector>
#include "page.h"
#include "classifier.h"

using namespace std;

void PrintBestMatches(vector<Blob>& vb, int n=1, bool print_dists=false){
    int n_blbs = vb.size();
    cout << endl;
    cout.setf(ios::fixed);

    for (int i=0; i<n_blbs; ++i){
    	cout << i+1 << " = ";
    	vb[i].PrintBestMatches(n, print_dists);
    }
}

int main(int    argc,     char **argv){
    if (argc < 2){
        cout << "Usage:\nbanti filename.tif [debug_flag] [write_boxes]"
             << "\ndebug_flag(0-7)"
             << "\n   0 Do nothing"
             << "\n   1 Print Histograms (Use even number to avoid this)"
             << "\n   2 Print Line Separation, Baselines, Toplines etc."
             << "\n   4 Show Cool Images"
             << "\n  You can also give sums of the above numbers\n";
        return -1;
    }

    string filename(argv[1]);
    Page mypage;

    cout << "Processing "<< filename;
    if (mypage.OpenImage(filename) != 0)
    	return -1;
    mypage.FilterNoise();
    mypage.CalcHist();
    mypage.FindBaseLines();
    mypage.SeperateLines();
    mypage.MorphAll();
    mypage.ProcessLines();

    vector<Blob> blobs;
    mypage.LoadBlobs(blobs);

    if (argc > 2)
        mypage.DebugDisplay(cout, atoi(argv[2]));

    string exe(argv[0]);
    Classifier lda(exe.substr(0, exe.find_last_of('/')));
    lda.PopulateFeatures(blobs);
    PrintBestMatches(blobs, 5, true);

    if (argc > 3){
        ofstream fbox;
        string boxfilename = filename.substr( 0, filename.find_last_of( '.' ) )
                                + string(".box");
        fbox.open(boxfilename.c_str());
        for (unsigned int i=0; i<blobs.size(); i++)
        	blobs[i].PrintBoxInfo(fbox, mypage.Height());
        fbox.close();
    }

    cout << endl;
    return 0;
}

