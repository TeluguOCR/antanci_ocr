/*
 * Classifier.h
 *
 *  Created on: Sep 19, 2012
 *      Author: rakesha
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_
#include "allheaders.h"
#include "config_banti.h"
#include "blob.h"
#include <string>
#include <vector>

extern std::vector<std::string> char_codes;//[NCLASSES];

class Classifier {
    /* IMPORTANT:
     * Matlab dumps matrices in to bin files by alternating the last column least
     * e.g. for 3x2 array
     *              (1,1) (2,1) (3,1)
     *              (1,2) (2,2) (3,2)
     * e.g. for 2x2x3
     *             (1,1,1) (2,1,1)
     *             (1,2,1) (2,2,1)
     *             (1,1,2) (2,1,2)
     *             (1,2,2) (2,2,2)
     *             (1,1,3) (2,1,3)
     *             (1,2,3) (2,2,3)
     *  So when read in to C they are transposed !
     */
	/* TODO: Change to Double Precision and move to Heap (not Stack)
	 *
	 * e.g.:-
	 * typedef int each2d[3][4];
	 * each2d *a3d = new each2d[10];
	 * a3d[ from 0 to 9 ][ from 0 to 2 ][ from 0 to 3 ] = 42;
	 * delete [] a3d;
	 *
	 * typedef double _nfeats_row[NFEATS];
	 * _nfeats_row *neg_sqroot_cov;
	 * _nfeats_row *sphered_means;
	 * sphered_means = new _nfeats_row[NCLASSES];
	 * neg_sqroot_cov = new _nfeats_row[NFEATS];
	 *
	 * typedef double _std_sz_nfeats_row[STD_SZ][NFEATS];
	 * _std_sz_nfeats_row canonical_pics;
	 * canonical_pics = new _std_sz_nfeats_row[STD_SZ];
	 *
	 * delete [] neg_sqroot_cov;
	 * delete [] sphered_means;
	 * delete [] canonical_pics;
	 *
	 * */

	l_float32 neg_sqroot_cov[NFEATS][NFEATS];
    l_float32 sphered_means[NCLASSES][NFEATS];
    l_float32 canonical_pics[STD_SZ][STD_SZ][NFEATS];
    inline float GetIthFeature(PIX *p, int iFeat);

public:

    Classifier(std::string dir);
    virtual ~Classifier();
    void PopulateFeatures(Blob& blob);
    void PopulateFeatures(vector<Blob>& blob);
};

#endif /* CLASSIFIER_H_ */
