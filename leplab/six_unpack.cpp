#include <iostream>
#include <fstream>
#include "allheaders.h"
#include <sstream>
#include <string>

using namespace std;
const int WORD_SZ = 32;

string ExtractFilename( const string& path ){
  return path.substr( path.find_last_of( '/' ) +1 );
}

string ChangeExtension( const string& path, const string& ext ){
  string filename = ExtractFilename( path );
  return filename.substr( 0, filename.find_last_of( '.' ) ) +ext;
}

string RemoveExtension(const string& filename){
    return filename.substr(0, filename.find_last_of('.'));
}

int main(int    argc,     char **argv)
{
    // Initials
    cout << "Here we go!" << endl;
    string bfnt;                //Base File Name for Temp

    // Open the image
    if (argc < 2){
        cout << "Usage:\n  "
            << argv[0] << "filename.box\n\t";
        return 1;
    }
    bfnt = string("/tmp/") + RemoveExtension(ExtractFilename(argv[1]));


    // Open Box File
    ifstream boxes(argv[1]);
    string box;
    string id, sixpack;
    int x, y, wd, ht, baseline,  topline, line, word, num = 0;

    while(getline(boxes, box)){
        istringstream boxss(box);
        boxss >> id >> x >> y >> wd >> ht 
            >> baseline >> topline 
            >> line >> word 
            >> sixpack;

            PIX* pixw = pixCreate(wd, ht, 1);
            //Decode sixpack and write
            int wpl = pixw->wpl;
            for(int ip = 0; ip < ht*wd; ++ip){  // Easier way is to use pixSetPixel(x,top,1)
                int row = int(ip/wd);
                int col = ip % wd;
                int word = int(col/WORD_SZ);
                int rbit = WORD_SZ - 1 - col % WORD_SZ;
                int is = int(ip/6);
                bool val = (sixpack[is] - '0') & (1 << (5 - (ip%6)));
                pixw->data[row*wpl + word] |= (val<<rbit);
            }

        pixSetResolution(pixw, 300, 300);
        stringstream fn, fn2;
        fn << bfnt << '_' << ++num // << '_' << ununify(id)
            << '_' << (y+ht-1-baseline) << '_' << (y-topline) << ".six.tiff";
        cout << num << ": " << fn.str()  << endl;
        pixWrite(fn.str().c_str(), pixw, IFF_TIFF_G4);
        pixDestroy(&pixw);
    }

    pixDisplayMultiple((bfnt+"*").c_str());
    return 0;
}
