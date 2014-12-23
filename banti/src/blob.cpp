/*
 * letter.cpp
 *
 *  Created on: Sep 20, 2012
 *      Author: raka
 */
#include "blob.h"
#include "math_utils.h"
#include "classifier.h"

Blob::Blob():best_matches_(NCLASSES) {
    box_ = NULL;
    pix_ = NULL;
    pix64_ = NULL;
    column_id_ = line_id_ = word_id_ = base_at_ = letter_ht_ = -1;
    aspect_ratio_ = -3.14;
    populated_= false;
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

string Blob::PackSix(){
	// Packs the blob into an Ascii string where each character holds six pixels
	// of the blob. The characters used are in the range [0,o] = [0x30, 0x70)
    int ht = pix_->h;
    int wd = pix_->w;
    int wpl = pix_->wpl;
    string s(1+int((1+ht*wd)/6), '0');

    for(int ip = 0; ip < ht*wd; ++ip){
    	int row = int(ip/wd);
    	int col = ip % wd;
    	int word = int(col/WORD_SZ);
    	int rbit = WORD_SZ - 1 - col % WORD_SZ;
    	bool val = pix_->data[row*wpl + word] & (1<<rbit);

    	int is = int(ip/6);
    	s[is] += val << (5 - (ip%6));
    }
    return s;
}

void Blob::PrintBoxInfo(ostream& out, int ht, bool tesseract_style){
	string match;
	if (populated_)
		match = char_codes[best_matches_[0]];
	else
		match = "?";
	if (!tesseract_style){
            out
	    	<< match << " "
			<< box_->x << " " << box_->y << " "
			<< box_->w << " " <<  box_->h << " "
			<< base_at_ << " " << base_at_ - letter_ht_ << " "
	    	<< line_id_ << " "  << word_id_ << " "
	    	<< PackSix()
	    	;
	}else{
            out
            << match << " "
            << box_->x << " " << abs(ht - (box_->y + box_->h - 1)) << " "
            << box_->x + box_->w - 1 << " " << abs(ht - box_->y) << " "
            ;
	}
    out << endl;
}
