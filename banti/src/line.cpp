/*
 * line.cpp
 *
 *  Created on: Sep 2, 2012
 *      Author: raka
 */

#include "line.h"
#include <iostream>
using namespace std;

Line::Line(){
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
                int line_id, int col_id, int letter_ht){
    top_ = top;
    bottom_ = bottom;
    base_line_ = base_line;
    box_line_ = boxCreate(0, top_, mother->w, bottom_-top_);
    pix_line_ = pixClipRectangle(mother, box_line_, NULL);
    line_id_ = line_id;
    col_id_ = col_id;
    letter_ht_ = letter_ht;
}

void Line::FindWordBoxes(){
    pix_words_ = pixCloseBrick(NULL, pix_line_, letter_ht_>>1, letter_ht_>>1);
    BOXA* tmp_boxa1 = pixConnCompBB(pix_words_, 4);
    BOXA* tmp_boxa2 = boxaSelectBySize(tmp_boxa1, letter_ht_>>2, letter_ht_>>2,
                                L_SELECT_IF_BOTH, L_SELECT_IF_GTE, NULL);
    BOXA* tmp_boxa3 = boxaSort(tmp_boxa2, L_SORT_BY_X, L_SORT_INCREASING, NULL);
    boxaDestroy(&tmp_boxa1);
    boxaDestroy(&tmp_boxa2);
    if (0){
        word_boxes_ = CheckOverlapBoxes(tmp_boxa3);
        boxaDestroy(&tmp_boxa3);
    }else
        word_boxes_ = tmp_boxa3;
    num_words_ = boxaGetCount(word_boxes_);
}

void Line::FindLetters(){
    PIXA *pixa_letters_tmp1, *pixa_letters_tmp2;
    BOXA* letter_boxes = pixConnComp(pix_line_, &pixa_letters_tmp1, 8);

    // Filter the letters_ and combine boxes if necessary
    // TODO: Combine smaller dots into bigger boxes
    pixa_letters_tmp2 = pixaSelectBySize(pixa_letters_tmp1,
                                        letter_ht_>>2, letter_ht_>>2,
                                        L_SELECT_IF_BOTH, L_SELECT_IF_GTE, NULL);
    pixa_letters_ = pixaSort  (pixa_letters_tmp2, L_SORT_BY_X, L_SORT_INCREASING,
                                                                NULL, L_CLONE);
    boxaDestroy(&letter_boxes);
    pixaDestroy(&pixa_letters_tmp1);
    pixaDestroy(&pixa_letters_tmp2);

    // By now the letters_ are frozen so add the letters_
    num_letters_ = pixaGetCount(pixa_letters_);
}

void Line::LoadBlobs(vector<Blob>::iterator& itr){
    for (int i = 0; i < num_letters_; i++) {
        // Which word do we belong
        BOX* box = pixaGetBox(pixa_letters_, i, L_CLONE);
        int word_id = 0, mindist = pixGetWidth(pix_line_);
        float centerx_box, dummy, centerx_word;
        boxGetCenter(box, &centerx_box, &dummy);
        for (int iw=0; iw < num_words_; ++iw){
            BOX* wbox = boxaGetBox(word_boxes_, iw, L_CLONE);
            boxGetCenter(wbox, &centerx_word, &dummy);
            int dist = abs(centerx_word - centerx_box);
            if (dist < mindist){
                mindist = dist;
                word_id = iw;
            }
            boxDestroy(&wbox);
        }

        // Add the letter
        box->y = box->y + top_;
        itr->Init(pixaGetPix(pixa_letters_, i, L_CLONE),
                box, col_id_, line_id_, word_id, base_line_, letter_ht_);
        ++itr;
    }
}

void Line::ProcessLine(){
    FindWordBoxes();
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

void Line::DisplayLetters(){
    PIX* pix_debug;
#if 0
    pix_debug = pixCopy(NULL, pix_line_);
    BOX* ibox;
    /* Draw outline of each c.c. box_line_ */
    for (int i = 0; i < num_words_; i++) {
        ibox = boxaGetBox(word_boxes_, i, L_CLONE);
        pixRenderBox(pix_debug, ibox, 1, L_FLIP_PIXELS);
        boxDestroy(&ibox);
    }
#else
    pix_debug = pixaDisplayOnLattice(pixa_letters_, 65, 65);
#endif
    pixDisplayWriteFormat(pix_debug, 1, IFF_PNG);
    pixDestroy(&pix_debug);
}

void Line::PrintSampleLetter(int letter){
    PIX *pix = pixaGetPix(pixa_letters_, letter, L_CLONE);
    for (int row = 0; row < pix->h; ++row){
        cout << endl;
        for (int word = 0; word < pix->wpl; ++word)
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
