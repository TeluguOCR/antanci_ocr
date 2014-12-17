#include <iostream>
#include <fstream>
#include "allheaders.h"

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
    float angle, conf;
    PIX *pixs, *pixtemp;
    string bfnt;                //Base File Name for Temp

    // Open the image
    if (argc < 4){
        cout << "Usage:\n  "
            << argv[0] << " image.tif preprocess morphsequence\n\t"
            << "preprocess: 0-Nothing, 1-Median Filter, 2-Deskew\n\t"
            << "morphsequence: e.g:- b32 + o1.3 + C3.1 + r23 + e2.2 + D3.2 + X4\n\t  "
            << "b32: Add a 32 pixel border around the input image \n\t  "
            << "o1.3: Opening with vert sel of length 3 (e.g., 1 x 3) \n\t  "
            << "C3.1: Closing with horiz sel of length 3  (e.g., 3 x 1)\n\t  "
            << "r23: Two successive 2x2 reductions with rank 2 in the first\n\t  "
            << "and rank 3 in the second.  The result is a 4x reduced pix.\n\t  "
            << "e2.2: Erosion with a 2x2 sel (origin will be at x,y: 0,0)\n\t  "
            << "d3.2: Dilation with a 3x2 sel (origin will be at x,y: 1,0)\n\t  "
            << "X4: 4x replicative expansion, back to original resolution\n\n";
        return 1;
    }
    pixs = pixRead(argv[1]);
    if(!pixs){
        cout << "\nCould not open " << argv[1]  <<endl;
        return 1;
    }
    bfnt = string("/tmp/") + RemoveExtension(ExtractFilename(argv[1]));

    unsigned int preprocess = atoi(argv[2]);
    string morphsequence(argv[3]);

    // Print basic pix info.
    pixGetDimensions(pixs, &w, &h, &d);
    pixGetResolution(pixs, &xres, &yres);
    cout << w << " " << h << " " << d << " " << xres << " " << yres << endl;

    if (preprocess & 1){
        // Median Filter
        pixtemp = pixBlockrank(pixs, NULL, 1, 1, 0.5);
        pixDestroy(&pixs);  pixs = pixtemp;
    }

    if (preprocess & 2){
        // Deskew
        pixtemp = pixFindSkewAndDeskew(pixs, 1, &angle, &conf);
        pixDestroy(&pixs);  pixs = pixtemp;
        cout << "Angle: " << angle << "\nConfidence : " << conf << endl;
    }
    pixWrite((bfnt+".tiff").c_str(), pixs, IFF_TIFF_G4);

    // MorphSequence
    pixtemp = pixMorphSequence(pixs, argv[3], 0);
    pixWrite((bfnt + "." + morphsequence + string(".tif")).c_str(), 
                pixtemp, IFF_TIFF_G4);
    pixDestroy(&pixtemp);

    pixDisplayMultiple((bfnt+"*").c_str());

    return 0;
}
