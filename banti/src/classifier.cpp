/*
 * Classifier.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: rakesha
 */

#if BANTI_DEBUG
#include <iostream>
#endif
#include <fstream>
#include <assert.h>
#include <cmath>
#include "classifier.h"
#include "math_utils.h"

using namespace std;

vector<string> char_codes;

Classifier::Classifier(string dir){
    string nfw = dir + "/data/wr.bin";
    string nfs = dir + "/data/sm.bin";
    string nfc = dir + "/data/cp.bin";

    ifstream fw;
    fw.open(nfw.c_str());
    if(!fw)
    	cerr << "\nClassifer could not open " << nfw;
	else{
		fw.read((char*)neg_sqroot_cov, NFEATS*NFEATS*BYTES_PER_WORD);
		fw.close();
	}

    ifstream fs;
    fs.open(nfs.c_str());
    if(!fs)
    	cerr << "\nClassifer could not open " << nfs;
    else{
    	fs.read((char*)sphered_means, NCLASSES*NFEATS*BYTES_PER_WORD);
    	fs.close();
    }

    ifstream fc;
    fc.open(nfc.c_str());
    if(!fc)
    	cerr << "\nClassifer could not open " << nfc;
    else{
		fc.read((char*)canonical_pics, NFEATS*STD_SZ*STD_SZ*BYTES_PER_WORD);
		fc.close();
    }

    string name_char_class_file = dir +"/data/charcodes.txt", s;
    ifstream f_chars;
    f_chars.open(name_char_class_file.c_str());
    if(!f_chars)
    	cerr << "\nClassifer could not open " << name_char_class_file;
    else{
		while (f_chars >> s)
			char_codes.push_back(s);
		f_chars.close();
    }
}

Classifier::~Classifier() {
}


inline float Classifier::GetIthFeature(PIX* p, int iFeat){
    /* IMPORTANT
     * In Matlab a black is 1 & white is 0 so there is an negation in the
     * if statement below.
     * Also Matlab stores its matrices in binary files by altering
     * the last column least. ie. It stores A(:,:,1) followed by A(:,:,2) etc.
     * So Indices are reversed and iFeat becomes last index as opposed to first!
     */
    assert (p->wpl == WPL);
    assert (p->h == STD_SZ);
    float sum = 0;
    for (unsigned int row = 0; row < p->h; ++row)
        for (unsigned int word = 0; word < p->wpl; ++word)
        for (int rbit = WORD_SZ-1, lbit = 0; rbit >= 0; --rbit, ++lbit)
            if (!(p->data[row*p->wpl + word] & (1<<rbit) ) )
                sum += canonical_pics[word*32 + lbit][row][iFeat];
    return sum;
}

void Classifier::PopulateFeatures(Blob& blob){
    // Features from CV Images
    for (int i=0; i < NFEATS; ++i)
        blob.features_[i] = GetIthFeature(blob.pix64_, i);

    // Rotated Features
    for (int i=0; i < NFEATS; ++i){
        blob.rot_feats_[i] = 0;
        for (int j = 0; j < NFEATS; j++)
            blob.rot_feats_[i] += neg_sqroot_cov[j][i] * blob.features_[j];
    }

    // Distances
    for (int i=0; i < NCLASSES; ++i){
        blob.sq_dist_to_means_[i] = 0;
        for (int j=0; j<NFEATS; ++j)
            blob.sq_dist_to_means_[i] += pow(blob.rot_feats_[j] - sphered_means[i][j], 2);
    }

    // Sort
    IncOrder(blob.sq_dist_to_means_, blob.best_matches_);
}

void Classifier::PopulateFeatures(vector<Blob>& blobs){
    int n_blbs = blobs.size();
    for (int i=0; i < n_blbs; ++i)
        PopulateFeatures(blobs[i]);
}
