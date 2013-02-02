/*
 * letter.h
 *
 *  Created on: Sep 20, 2012
 *      Author: raka
 */

#ifndef BANTI_LETTER_H_
#define BANTI_LETTER_H_
#include <iostream>
#include <vector>
#include "allheaders.h"
#include "config_banti.h"

using namespace std;

class Blob{
    int   column_id_;
    int   line_id_;
    int   word_id_;
    int   base_at_;
    int   letter_ht_;
    float aspect_ratio_;

    BOX* box_;
    PIX* pix_;
    PIX* pix64_;

    float features_[NFEATS];
    float rot_feats_[NFEATS];
    float sq_dist_to_means_[NCLASSES];

    vector<size_t> best_matches_;

    friend class Classifier;
public:
    Blob();
    ~Blob();
    void Init(  PIX* pix, BOX* box,
                int col_id, int line_id, int word_id,
                int base_at, int letter_ht);
    int FindBestMatch();
    void PrintBestMatches(int n, bool print_dists);
    void PrintBoxInfo(ostream& out, int ht);
};

#endif /* LETTER_H_ */
