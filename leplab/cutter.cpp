#include <iostream>
#include <fstream>
#include "allheaders.h"
#include <sstream>
#include <string>

using namespace std;

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
    int w, h, d, xres, yres;
    PIX *pixs;
    string bfnt;                //Base File Name for Temp

    // Open the image
    if (argc < 2){
        cout << "Usage:\n  "
            << argv[0] << " filename.box\n\t";
        return 1;
    }
    string img_file_name;
    if (argc > 2)
        img_file_name = string(argv[2]);
    else{
        string s(argv[1]);
        img_file_name = s.replace(s.find(".box"), 4, ".tif");        
    }

    pixs = pixRead(img_file_name.c_str());
    if(!pixs){
        cout << "\nCould not open " << img_file_name  <<endl;
        return 1;
    }
    bfnt = string("/tmp/") + RemoveExtension(ExtractFilename(argv[1]));

    // Print basic pix info.
    pixGetDimensions(pixs, &w, &h, &d);
    pixGetResolution(pixs, &xres, &yres);
    cout << w << " " << h << " " << d << " " << xres << " " << yres << endl;

    // Open Box File
    ifstream boxes(argv[1]);
    string box;
    string id, sixpack;
    int x, bot, y, right, base,  topline, line, word, num = 0;
    BOX* charWindow = boxCreate(0, 0, 0, 0);

    while(getline(boxes, box)){
        istringstream boxss(box);
        boxss >> id >> x >> bot >> right >> y 
            >> base >> topline 
            >> line >> word 
            >> sixpack;
        boxSetGeometry(charWindow, x, y, right-x+1, bot-y+1);
        PIX* pixw = pixClipRectangle(pixs, charWindow, NULL);
        pixSetResolution(pixw, 300, 300);
        stringstream fn, fn2;
        fn << bfnt << img_file_name << '_' << ++num << '_' << id 
            << '_' << (base-x) << '_' << (topline-x) << ".tiff";
        fn2 << bfnt << ++num << ".tiff";
        pixWrite(fn2.str().c_str(), pixw, IFF_TIFF_G4);
        pixDestroy(&pixw);
    }

    boxDestroy(&charWindow);
    pixDestroy(&pixs);
    pixDisplayMultiple((bfnt+"*").c_str());
    return 0;
}
