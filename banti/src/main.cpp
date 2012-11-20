#include <fstream>
#include <iostream>
#include <vector>
#include "page.h"
#include "classifier.h"

using namespace std;

int main(int    argc,     char **argv){
    if (argc < 2){
        cout << "Usage:\nbanti filename.tif [debug_flag] [write_boxes]"
             << "\ndebug_flag(0-31)"
             << "\n   0 Do nothing"
             << "\n   1 Print Histograms"
             << "\n   2 Print Line Separation, Baselines, Toplines etc."
             << "\n   4 Show Morphed Images"
             << "\n   8 Show Baselines Image"
             << "\n   16 Show Lattice Image of Letters in Line"
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

    if (argc > 3){
        ofstream fbox;
        string boxfilename = filename.substr( 0, filename.find_last_of( '.' ) )
                                + string(".box");
        fbox.open(boxfilename.c_str());
        mypage.PrintBoxInfo(fbox);
        fbox.close();
    }
    PrintBlobsInfo(blobs);

    cout << "\n\nSizes"
    		<<"\nchar "<<sizeof(char)*8
    		<<"\nshort "<<sizeof(short)*8
    		<<"\nint "<<sizeof(int)*8
    		<<"\nlong "<<sizeof(long)*8
    		<<"\nfloat "<<sizeof(float)*8
    		<<"\ndouble "<<sizeof(double)*8
    		<<"\nTotal for classifier" << (NFEATS*NFEATS + NCLASSES*NFEATS + STD_SZ*STD_SZ*NFEATS)*sizeof(float);

    string exe(argv[0]);
    Classifier lda(exe.substr(0, exe.find_last_of('/')));
    lda.PopulateFeatures(blobs);
    PrintBestMatch(blobs);

    cout << endl;
    return 0;
}

/* CC Prog Files
 * cctest1.c 		Demonstrates using pixConnComp() and testing its speed.
 * ccthin1_reg.c 	Tests the "best" cc-preserving thinning functions.
 * 					Displays all the strong cc-preserving 3x3 Sels.
 * ccthin2_reg.c 	Tests the examples in pixThinExamples().
 * compfilter_reg.c Tests filters that select components based on size,
 * 			    	areaPerimRatio, areaFraction, etc.
 * conncomp_reg.c 	Regression test for connected components (both 4 and 8 connected),
 *	 				including regeneration of the original image from the components.
 * 					This is also an implicit test of rasterop.
 * pixa1_reg.c 		Tests removal of components using: pixConnComp() and
 * 			    	pixSelectBySize().
 * splitcomp_reg.c 	Regression test for splittings of a single component and for an
 * 		    		image composed of several components, using different components
 * 	    			and parameters.
 * ccbordtest.c 	Comprehensive test for border-following representations of
 *  				binary images.
 */

/* Formatted I/O
 * mtifftest.c 	Exploration of multi-page tiff I/O and writing special tiff tags.
 */

/* Fundamental Data Structures for Computation
 * extrema_reg.c 	Tests procedure for locating extrema (minima and maxima) of a sampled function.
 *
 */

/* Image Display
 * xvdisp.c 	Display an image with xv.
 */

/* Scaling
 * scaletest1.c 	Exploration of image scaling techniques.
 * scaletest2.c 	Exploration of scale-to-gray and color scaling.
 * expand_reg.c 	Tests of various image expansion techniques.
 * scale_reg.c 	This tests a number of scaling operations, through the pixScale() interface.
 */

/* Line Graphics and Special Output
 * graphicstest.c 	Demonstrates line graphics using: pixRenderLineArb(), pixRenderLineBlend(), pixRenderBoxArb(), pixRenderBoxaBlend(), and pixRenderPolylineBlend().
 */


/* pixGetWordBoxesInTextlines
    pixt2 = pixWordMaskByDilation(pixt1, maxsize, NULL);

    boxa1 = pixConnComp(pixt2, NULL, 8);
    boxa2 = boxaSelectBySize(boxa1, minwidth, minheight,
                             L_SELECT_IF_BOTH, L_SELECT_IF_GTE, NULL);
    boxa3 = boxaSelectBySize(boxa2, maxwidth, maxheight,
                             L_SELECT_IF_BOTH, L_SELECT_IF_LTE, NULL);
    baa = boxaSort2d(boxa3, NULL, 3, -5, 5);
    boxad = boxaaFlattenToBoxa(baa, &nai, L_CLONE);
 */

/* Connected components and Scaling
 *     boxa = pixConnComp(pixs, &pixa, 4);
 *     pixd[i] = pixScaleBinary(pixa[i], 64.0/pixa[i].width, 64.0/pixa[i].height);
 *
 */
