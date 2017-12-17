#ifndef _otx_file_h_
#define _otx_file_h_

bool initOtxWriter(const char* path);
bool addFile2Otx(const char* path);
void closeOtxWriter();

int initOtxReader(const char* path);
int locateFileInOtx(const char* path);
bool extractFileFromOtx(unsigned int file_idx);
void closeOtxReader();

#endif
