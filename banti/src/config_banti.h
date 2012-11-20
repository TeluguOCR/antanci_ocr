/*
 * config_banti.h
 *
 *  Created on: Sep 20, 2012
 *      Author: raka
 */

#ifndef CONFIG_BANTI_H_
#define CONFIG_BANTI_H_

#define BANTI_DEBUG 1
#define BANTI_DEBUG_ARCHIVE 0

#define NFEATS 451
#define NCLASSES 385
#define STD_SZ 64
#define WORD_SZ 32
#define WPL STD_SZ/WORD_SZ         // Words Per Line in an Blob Image 64/32 = 2
#define BYTES_PER_WORD WORD_SZ/8


#endif /* CONFIG_BANTI_H_ */
