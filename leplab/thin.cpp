#include "allheaders.h"
#include <iostream>
using namespace std;

main(int    argc,
     char **argv)
{
l_int32      index, maxiters, type;
PIX         *pixs, *pixd, *pixt;
PIXA        *pixa;

    if (argc != 2 && argc != 4){
        cout << " Syntax: thin in.tif [index maxiters]\n";
        return -1;
    }

    pixDisplayWrite(NULL, 0);
    if ((pixs = pixRead(argv[1])) == NULL){
        cout << "\nCould not open " << argv[1] << endl;
        return -1;
    }
    pixs = pixBlockrank(pixs, NULL, 1, 1, 0.5);
    pixDisplayWrite(pixs, 1);

        /* Just do one of the examples */
    if (argc == 4) {
        index = atoi(argv[2]);
        maxiters = atoi(argv[3]);
        if (index <= 7)
            type = L_THIN_FG;
        else
            type = L_THIN_BG;
        pixt = pixThinExamples(pixs, type, index, maxiters,
                               "/tmp/junksels.png");
        pixDisplay(pixt, 100, 100);
        pixDisplayWrite(pixt, 1);
        pixDestroy(&pixt);
        pixDisplayMultiple("/tmp/junk_write_display*");
        return 0;
    }

        /* Do all the examples */
    pixt = pixThinExamples(pixs, L_THIN_FG, 1, 0, "/tmp/junksel_example1.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_FG, 2, 0, "/tmp/junksel_example2.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_FG, 3, 0, "/tmp/junksel_example3.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_FG, 4, 0, "/tmp/junksel_example4.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_FG, 5, 0, "/tmp/junksel_example5.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_FG, 6, 0, "/tmp/junksel_example6.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_FG, 7, 0, "/tmp/junksel_example7.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_BG, 8, 5, "/tmp/junksel_example8.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);
    pixt = pixThinExamples(pixs, L_THIN_BG, 9, 5, "/tmp/junksel_example9.png");
    pixDisplayWrite(pixt, 1);
    pixDestroy(&pixt);

        /* Display the thinning results */
    pixa = pixaReadFiles("/tmp", "junk_write_display");
    pixd = pixaDisplayTiledAndScaled(pixa, 8, 500, 1, 0, 25, 2);
    pixWrite("/tmp/junktiles.jpg", pixd, IFF_JFIF_JPEG);
    pixDestroy(&pixd);
    pixaDestroy(&pixa);

        /* Display the sels used in the examples */
    pixa = pixaReadFiles("/tmp", "junksel_example");
    pixd = pixaDisplayTiledInRows(pixa, 1, 500, 1.0, 0, 50, 2);
    pixWrite("/tmp/junksels.png", pixd, IFF_PNG);
    pixDestroy(&pixd);
    pixaDestroy(&pixa);
    pixDestroy(&pixs);

    pixDisplayMultiple("/tmp/junk_write_display*");
    return 0;
}


