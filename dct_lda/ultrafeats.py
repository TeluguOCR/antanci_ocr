#! /usr/bin/env python

# Telugu OCR project
# Opens the training img set and generates the best basis to represent the 
# given set of images
import numpy as np
from PIL import Image 
from scipy import fftpack as fft
from scipy import misc
import matplotlib.pyplot as plt
import sys

IDX, LEFT, BOT, RIGHT, TOP, BASE = 1, 2, 3, 4, 5, 6

STD_SZ = 64
nCVs = 385
file_names = ['p385', 'g385', 'r385']
nFonts = len(file_names)

DUPS = 100
NOISE_LEVEL = 0.1
ROT_MAX = 4

for font in file_names:
    font = "data/"+font
    box_file = open(font+".box")
    img = misc.imread(font+".png")
    ht = img.shape[1]
    
    for box in box_file:
        i, l, b, r, t, d = [t(s) for t,s in zip((int,int,int,int, int,int), box.split(" "))]
        b,  t = ht - b, ht - t  
        for row in range(t, b):
            for col in range(l, r):
                print img[row, col],
            print
        glyph = misc.imresize(img[l:r, t:b], (STD_SZ, STD_SZ), 'nearest')
        print glyph.shape
        for row in range(STD_SZ):
            for col in range(STD_SZ):
                print glyph[row, col],
            print
        sys.exit()
     