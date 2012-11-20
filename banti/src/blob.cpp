/*
 * letter.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: raka
 */
#include "blob.h"
#include "math_utils.h"

Blob::Blob() {
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

int Blob::FindBestMatch(){
    vector<float> vec_sq_dists (sq_dist_to_means_, sq_dist_to_means_ + NCLASSES);
    float min;
    VecMin(vec_sq_dists, min, best_match_);
    return best_match_;
}

Blob::~Blob() {
    if (pix_)
        pixDestroy(&pix_);
    if (pix64_)
        pixDestroy(&pix64_);
    if (box_)
        boxDestroy(&box_);
}

void PrintBlobsInfo(vector<Blob>& vb){
#if BANTI_DEBUG_ARCHIVE
    int n_blbs = vb.size();
    for (int i=0; i<n_blbs; ++i){
      cout << vb[i].line_id_ << " "
              << vb[i].word_id_ << " "
              << vb[i].box_->x << " "
              << vb[i].box_->y << " "
              << endl;
    }
#endif
}

void PrintBestMatch(vector<Blob>& vb){
    int n_blbs = vb.size();
    cout << endl;

    for (int i=0; i<n_blbs; ++i){
      cout << i+1 << " = " << 1+vb[i].FindBestMatch() << endl;
    }
}
