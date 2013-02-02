/*
 * letter.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: raka
 */
#include "blob.h"
#include "math_utils.h"
#include "classifier.h"
#include <iomanip>

Blob::Blob():best_matches_(NCLASSES) {
    box_ = NULL;
    pix_ = NULL;
    pix64_ = NULL;
    column_id_ = line_id_ = word_id_ = base_at_ = letter_ht_ = -1;
    aspect_ratio_ = -3.14;
}

void Blob::Init(PIX* pix, BOX* box, int col_id, int line_id, int word_id,
        int base_at, int letter_ht) {
    pix_ = pix;
    box_ = box;
    column_id_ = col_id;
    line_id_ = line_id;
    word_id_ = word_id;
    base_at_ = base_at;
    letter_ht_ = letter_ht;
    aspect_ratio_ = (float) box_->w / box_->h;

    pix64_ = pixScaleBinary(pix, 64.0 / pix->w, 64.0 / pix->h);
}

Blob::~Blob() {
    if (pix_)
        pixDestroy(&pix_);
    if (pix64_)
        pixDestroy(&pix64_);
    if (box_)
        boxDestroy(&box_);
}

void Blob::PrintBestMatches(int n=1, bool print_dists=false){
    for (int j=0; j<n; ++j){
    	size_t best = best_matches_[j];
		cout << "\t" << char_codes[best];

		if(print_dists)
		cout << "\t(" << setw(3) << 1+best << ") - "
		   << std::setprecision(0) << setw(10)
		   << sq_dist_to_means_[best_matches_[j]]
		   << "\n";
  }
}

void Blob::PrintBoxInfo(ostream& out, int ht){
    out  << char_codes[best_matches_[0]] << " "
		 << box_->x << " " << ht - (box_->y + box_->h - 1) << " "
		 << box_->x + box_->w - 1 << " " << ht - box_->y << " "
		 << base_at_ << endl;
}


