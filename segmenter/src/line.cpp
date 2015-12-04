/*
 * line.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: raka
 */

#include "line.h"
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

Line::Line(){
	training_mode_ = false;
	connection4or8_ = 0;
    box_line_ = NULL;
    word_boxes_ = NULL;
    pix_line_ = pix_words_ = NULL;
    pixa_letters_ = NULL;
}

Line::~Line(){
    if (pix_line_)
        pixDestroy(&pix_line_);
    if (pix_words_)
        pixDestroy(&pix_words_);
    if (box_line_)
        boxDestroy(&box_line_);
    if (word_boxes_)
        boxaDestroy(&word_boxes_);
    if (pixa_letters_)
        pixaDestroy(&pixa_letters_);
}

void Line::Init(int top, int bottom, int base_line, PIX* mother,
                int line_id, int col_id, int xht,
                bool training_mode, int connection4or8){
    top_ = top;
    bottom_ = bottom;
    base_line_ = base_line;
    box_line_ = boxCreate(0, top_, mother->w, bottom_-top_);
    pix_line_ = pixClipRectangle(mother, box_line_, NULL);
    line_id_ = line_id;
    col_id_ = col_id;
    xht_ = xht;
	training_mode_ = training_mode;
	connection4or8_ = connection4or8;
}

void Line::FindWordBoxesByMorphing(){
	pix_words_ = pixDilateBrick(NULL, pix_line_, 1, (xht_>>1)+2);
    pixCloseBrick(pix_words_, pix_words_, xht_ * .6, 1);
    BOXA* tmp_boxa1 = pixConnCompBB(pix_words_, connection4or8_);
    BOXA* tmp_boxa2 = NULL;
    if (training_mode_)
    	tmp_boxa2 = boxaCopy(tmp_boxa1, L_CLONE);
    else
    	tmp_boxa2 = boxaSelectBySize(tmp_boxa1, xht_>>3, xht_>>3,
                                L_SELECT_IF_BOTH, L_SELECT_IF_GTE, NULL);
    BOXA* tmp_boxa3 = boxaSort(tmp_boxa2, L_SORT_BY_X, L_SORT_INCREASING, NULL);

    word_boxes_ = boxaCopy(tmp_boxa3, L_CLONE);
    num_words_ = boxaGetCount(word_boxes_);
    boxaDestroy(&tmp_boxa1);
    boxaDestroy(&tmp_boxa2);
    boxaDestroy(&tmp_boxa3);
}

void Line::MergeContainedBoxes(PIXA* pixes){
	l_int32 container, containee = 1, contained;
	while(containee < pixes->n){
		BOX* neebox = pixaGetBox(pixes, containee, L_COPY);
		container = containee - 1;
		while(container >= 0){
			bool to_be_merged = false;
			BOX* nerbox = pixaGetBox(pixes, container, L_COPY);
			boxContains(nerbox, neebox, &contained);
			if (contained == 1){
				float relative_size = (float)(nerbox->w * nerbox->h)/(neebox->w * neebox->h);
				float area_fraction;
				PIX* p = pixaGetPix(pixes, containee, L_CLONE);
				pixFindAreaFraction(p, NULL, &area_fraction);
				pixDestroy(&p);
				if ((area_fraction < .0) && (area_fraction > .0)){
					cout << "Line_id " << line_id_ << " "
						 << container << " contains " << containee
						 << " relative_size " << relative_size
						 << " area_fraction " << area_fraction << endl ;
					PrintSampleLetter(container);
				}
				to_be_merged = (relative_size >= 15) || (area_fraction >= .625);
			}
			if (to_be_merged){
				PIX* nerpix = pixaGetPix(pixes, container, L_CLONE);
				PIX* neepix = pixaGetPix(pixes, containee, L_CLONE);
				PIX* newpix = pixAddBorderGeneral(neepix,
						neebox->x - nerbox->x, nerbox->x+nerbox->w - neebox->x -neebox->w,
						neebox->y - nerbox->y, nerbox->y+nerbox->h - neebox->y -neebox->h, 0);
				pixOr(nerpix, nerpix, newpix);
				pixDestroy(&nerpix);
				pixDestroy(&neepix);
				pixDestroy(&newpix);
				pixaRemovePix(pixes, containee);
				containee--;	// Because a pix has been removed, we need to step back
				break;
			}
			else
				container--;
			boxDestroy(&nerbox);
		}
		boxDestroy(&neebox);
		containee++;
	}
}

PIXA* SortGlyphsInAkshara(PIXA* pixes){
	BOXA* boxes = pixaGetBoxa(pixes, L_CLONE);
	vector<l_int32> idx(boxes->n);
	iota(idx.begin(), idx.end(), static_cast<l_int32>(0));
	sort(idx.begin(), idx.end(), [&boxes](l_int32 i, l_int32 j){
			l_int32 il, it, iw, ih, jl, jw, jt, jh, ir, jr;
			boxaGetBoxGeometry(boxes, i, &il, &it, &iw, &ih); ir = il + iw;
			boxaGetBoxGeometry(boxes, j, &jl, &jt, &jw, &jh); jr = jl + jw;
			l_int32 ol = ((ir >= jl) && (jr >= il)) * min(ir - jl, jr - il);
			l_int32 ol_percent = 100 * ol / min(iw, jw);
			if (ol_percent < 50)
				return il < jl;
			else
				return it + ih/2 < jt + jh/2;
			});
	NUMA* naindex = numaCreateFromIArray(&idx[0], boxes->n);
	PIXA* pixa_result = pixaSortByIndex(pixes, naindex, L_CLONE);
	boxaDestroy(&boxes);
	pixaDestroy(&pixes);
	return pixa_result;
}

void Line::FindLetters(){
    PIXA *pixa_letters_tmp1, *pixa_letters_tmp2;
    pixConnComp(pix_line_, &pixa_letters_tmp1, connection4or8_);

    if (training_mode_)
    	pixa_letters_tmp2 = pixaCopy(pixa_letters_tmp1, L_CLONE);
    else
    	pixa_letters_tmp2 = pixaSelectBySize(pixa_letters_tmp1,
                                        xht_>>2, xht_>>2,
                                        L_SELECT_IF_EITHER, L_SELECT_IF_GTE, NULL);
    pixa_letters_ = pixaSort(pixa_letters_tmp2, L_SORT_BY_X, L_SORT_INCREASING,
                                                                NULL, L_CLONE);
    MergeContainedBoxes(pixa_letters_);
    pixa_letters_ = SortGlyphsInAkshara(pixa_letters_);
    pixaDestroy(&pixa_letters_tmp1);
    pixaDestroy(&pixa_letters_tmp2);

    // By now the letters_ are frozen so add the letters_
    num_letters_ = pixaGetCount(pixa_letters_);
}

void Line::LoadBlobs(vector<Blob>::iterator& itr){
    for (int i = 0; i < num_letters_; i++) {
        // Which word do we belong
        BOX* box = pixaGetBox(pixa_letters_, i, L_CLONE);
        int word_id = -1, yes;
        float cx, cy;
        boxGetCenter(box, &cx, &cy);
        for (int iw=0; iw < num_words_; ++iw){
            BOX* wbox = boxaGetBox(word_boxes_, iw, L_CLONE);
            boxContainsPt(wbox, cx, cy, &yes);
            if (yes)
            	word_id = iw;
            boxDestroy(&wbox);
        }

        // Add the letter
        box->y = box->y + top_;
        itr->Init(pixaGetPix(pixa_letters_, i, L_CLONE),
                box, col_id_, line_id_, word_id, base_line_, xht_);
        ++itr;
    }
}

void Line::ProcessLine(){
    FindWordBoxesByMorphing();
    FindLetters();
}

BOXA* Line::CheckOverlapBoxes(BOXA* in_boxes){
    int     niters = 0;
    BOXA    *added_boxes;
    int     num_added, num_in = boxaGetCount(in_boxes);

    while (1) {  /* loop until no change from previous iteration */
        niters++;
        added_boxes = boxaCreate(num_in);
        for (int i_in = 0; i_in < num_in; i_in++) {
            BOX *in_box = boxaGetBox(in_boxes, i_in, L_COPY);
            if (i_in == 0) {
                boxaAddBox(added_boxes, in_box, L_INSERT);
                continue;
            }
            num_added = boxaGetCount(added_boxes);
            bool interfound = FALSE;
            for (int i_added = 0; i_added < num_added; i_added++) {
                BOX* added_box = boxaGetBox(added_boxes, i_added, L_CLONE);
                if (ShouldMixBoxes(in_box, added_box)) {
                    BOX* mix_box = boxBoundingRegion(in_box, added_box);
                    boxaReplaceBox(added_boxes, i_added, mix_box);
                    boxDestroy(&in_box);
                    boxDestroy(&added_box);
                    interfound = TRUE;
                    break;
                }
                boxDestroy(&added_box);
            }
            if (interfound == FALSE)
                boxaAddBox(added_boxes, in_box, L_INSERT);
        }
        num_added = boxaGetCount(added_boxes);
/*        fprintf(stderr, "%d iters: %d boxes\n", niters, num_added); */
        if (num_added == num_in)  /* we're done */
            break;
        else {
            num_in = num_added;
            boxaDestroy(&in_boxes);
            in_boxes = added_boxes;
        }
    }
    return added_boxes;
}

inline bool Line::ShouldMixBoxes(BOX* b1, BOX* b2){
    int l1 = b1->x, l2 = b2->x, r1 = l1+b1->w-1, r2 = l2+b2->w-1;
    return (r1 >= l2) && (r2 >= l1);
}

void Line::DisplayLettersLattice(){
    int wt, ht, max_wt = -1, max_ht = -1;
    // Find max character sizes before display
    for (int index = 0; index < num_letters_; index++) {
		pixaGetPixDimensions(pixa_letters_, index, &wt, &ht, NULL);
		if (wt > max_wt)	max_wt = wt;
		if (ht > max_ht)    max_ht = ht;
    }
	pixaDisplayOnLattice(pixa_letters_, max_wt, max_ht);
}

void Line::PrintColorLetters(PIX* pix_debug, unsigned char& index){
    int n = pixaGetCount(pixa_letters_);
    int xb, yb, wb, hb;
    for (int i = 0; i < n; i++) {
        index = 1 + (index % 254);
        pixaGetBoxGeometry(pixa_letters_, i, &xb, &yb, &wb, &hb);
        PIX* pixs = pixaGetPix(pixa_letters_, i, L_CLONE);
        PIX* pixt = pixConvert1To8(NULL, pixs, 0, index);
        pixRasterop(pix_debug, xb, yb, wb, hb, PIX_PAINT, pixt, 0, 0);
        pixDestroy(&pixs);
        pixDestroy(&pixt);
    }

    // Display Word Boxes
    BOX* ibox;
    for (int i = 0; i < num_words_; i++) {
        ibox = boxaGetBox(word_boxes_, i, L_CLONE);
        ibox->y += top_;
        pixRenderBoxArb(pix_debug, ibox, 1, 0,0,0);
        boxDestroy(&ibox);
    }
}

void Line::PrintSampleLetter(int letter){
    PIX *pix = pixaGetPix(pixa_letters_, letter, L_CLONE);
    for (l_uint32 row = 0; row < pix->h; ++row){
        cout << endl;
        for (l_uint32 word = 0; word < pix->wpl; ++word)
            for (int ibit=8*sizeof(l_uint32)-1; ibit >= 0 ; --ibit)
                if (pix->data[row*pix->wpl+word] & (1<<ibit))
                    cout << '#';
                else
                    cout << ' ';
    }
    cout << endl;
    pixDestroy(&pix);
}

void Line::PrintBoxes(ostream& fout, int ht){
    static int count = 0;
    for (int i = 0; i < num_letters_; i++){
       BOX* box = pixaGetBox(pixa_letters_, i, L_CLONE);
       fout << (++count) << " "
                << box->x << " " << ht-(box->y + box->h - 1) << " "
                << box->x + box->w - 1 << " " << ht-box->y << " "
                << base_line_ << endl;
       boxDestroy(&box);
    }
}
