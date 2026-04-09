
//{{BLOCK(store)

//======================================================================
//
//	store, 256x192@8, 
//	Transparent color : FF,00,FF
//	+ palette 256 entries, not compressed
//	+ bitmap not compressed
//	Total size: 512 + 49152 = 49664
//
//	Time-stamp: 2026-04-09, 14:06:39
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_STORE_H
#define GRIT_STORE_H

#define storeBitmapLen 49152
extern const unsigned int storeBitmap[12288];

#define storePalLen 512
extern const unsigned short storePal[256];

#endif // GRIT_STORE_H

//}}BLOCK(store)
