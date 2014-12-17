#include <iostream>
#include "allheaders.h"
using namespace std;

string ChangeExtension( const string& path, const string& ext ){
  return path.substr( 0, path.find_last_of( '.' ) ) + ext;
}

int main(int    argc,     char **argv)
{
    // Initials
    int wd, ht, depth, xres, yres;
    PIX *pixs, *pixt, *pixw;

    // Open the image
    if (argc < 2){
        cout << "Usage:\n  "
            << argv[0] << " image_file \n\t"
            << "Zealous crops an image (retaining a padding of 50)\n\t"
            << "Writes out a 1bpp Tiff file of same name\n\n";
        return -1;
    }
    pixs = pixRead(argv[1]);
    if(!pixs){
        cout << "\nCould not open " << argv[1]  <<endl;
        return 1;
    }
    // Print basic pix info.
    pixGetDimensions(pixs, &wd, &ht, &depth);
    pixGetResolution(pixs, &xres, &yres);
    cout << "Converting: "  << argv[1]
         << "\n\t" << wd << "x" << ht 
         << " " << depth 
         << "bpp res:" << xres; 

    if (depth > 1) {
        pixt = pixConvertTo1(pixs, 64);
        depth = pixGetDepth(pixt);
    }else
        pixt = pixClone(pixs);

    // Find actual ht and width
    int new_ht, new_wd, PADDING = 50;
    l_int32 count = -1;
    for (new_ht = ht - 1; new_ht >= 0; --new_ht){
        pixCountPixelsInRow(pixt, new_ht, &count, NULL);
        if (count > 0)
            break;
    }
    new_ht += min(ht - new_ht, PADDING + 1);

    // Counting columns is trickier
    NUMA* counts = pixCountPixelsByColumn(pixt);
    for (new_wd = wd - 1; new_wd >= 0; --new_wd){
        if (counts->array[new_wd] > 0)
            break;
    }
    numaDestroy(&counts);
    new_wd += min(wd - new_wd, PADDING + 1);

    cout << "\n\t" << new_wd << "x" << new_ht
         << "  " << depth 
         << "bpp res:" << 300; 

    BOX* cropWindow = boxCreate(0, 0, new_wd, new_ht);
    pixw = pixClipRectangle(pixt, cropWindow, NULL);
    pixSetResolution(pixw, 300, 300);
    pixWrite((ChangeExtension(argv[1], ".tif")).c_str(), pixw, IFF_TIFF_G4);

    boxDestroy(&cropWindow);
    pixDestroy(&pixw);
    pixDestroy(&pixs);
    pixDestroy(&pixt);
    cout <<  "\n";
    return 0;
}
