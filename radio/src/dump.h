#ifndef _DUMP_H_
#define _DUMP_H_

#if defined(DEBUG) || defined(CLI)
void dumpStart(unsigned int size);
void dumpBody(const uint8_t * data, unsigned int size);
void dumpEnd();
void dump(const uint8_t * data, unsigned int size);
#else
#define dump(data, size)
#endif

#endif // _DUMP_H_
