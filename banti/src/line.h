/*
 * line.h
 *
 *  Created on: Sep 2, 2012
 *      Author: raka
 */

#ifndef BANTI_LINE_H_
#define BANTI_LINE_H_
#include <vector>
#include <fstream>
#include "allheaders.h"
#include "blob.h"
using namespace std;

class Line{
    int line_id_, col_id_;
	BOX* box_line_;
	PIX* pix_line_;
	PIX* pix_words_;
	int num_words_;
	int num_letters_;
	BOXA* word_boxes_;
	PIXA* pixa_letters_;
	int top_, bottom_, base_line_;
    int letter_ht_;

	BOXA* CheckOverlapBoxes(BOXA* boxes);
	bool inline ShouldMixBoxes(BOX* b1, BOX* b2);
	void FindWordBoxes();
    void FindLetters();

public:
	Line();
    ~Line();
	void Init(int top, int bot, int base_line, PIX* mother, int line_id, int col_id, int letter_ht);

	void ProcessLine();
	void LoadBlobs(vector<Blob>::iterator& itr);

	inline int GetNumWords()                { return num_words_;	}
	inline int GetNumLetters()              { return num_letters_;  }
	void DisplayLetters();
	void PrintSampleLetter(int letter);
	void PrintBoxes(ostream& fout, int ht);
    //float DoImageInnerProduct64(int letter, float wts[64][64]);
};

#endif /* BANTI_LINE_H_ */
