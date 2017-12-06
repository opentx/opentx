#ifndef _otx_file_h_
#define _otx_file_h_

bool initOtxWriter(const char* path);
bool addFile2Otx(const char* path);
void closeOtxFile();

#endif
