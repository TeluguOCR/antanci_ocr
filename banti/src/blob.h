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
extern bool training_mode;

class Blob{
public:
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

    Blob();
    ~Blob();
    void Init(  PIX* pix, BOX* box,
                int col_id, int line_id, int word_id,
                int base_at, int letter_ht);
    void PrintBoxInfo(ostream& out, int ht, bool tesseract_style);
    string PackSix();
};

#endif /* LETTER_H_ */
