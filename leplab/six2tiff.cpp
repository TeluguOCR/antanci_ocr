#include "allheaders.h"

int main(int argc, char **argv)
{
    if (argc < 5)
        return -1;
    //Usage: six2tif wd ht name pic     

    int ht = atoi(argv[2]);
    int wd = atoi(argv[1]);
    char* sixpack = argv[4];

    PIX* pixw = pixCreate(wd, ht, 1);
    pixSetResolution(pixw, 300, 300);
    //Decode sixpack and write
    for(int x = 0; x < wd; ++x)
    for(int y = 0; y < ht; ++y){
        int ip = y * wd + x;
        int is = int(ip/6);
        char kar = sixpack[is];
        if (kar == 'z') kar = '`';
        bool val = (kar - '0') & (1 << (5 - (ip%6)));
        pixSetPixel(pixw, x, y, val);
    }
    pixWrite(argv[3], pixw, IFF_TIFF_G4);
    pixDestroy(&pixw);
    return 0;
}
