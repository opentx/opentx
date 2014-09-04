#include <stdlib.h>
#include <string.h>
#include "opentx.h"
#include "bin_allocator.h"


BinAllocator_slots1 slots1;
BinAllocator_slots2 slots2;
int SimulateMallocFailure = 0;


bool bin_free(void * ptr)
{
  //return TRUE if our
  return (slots1.free(ptr) || slots2.free(ptr));
}

void * bin_malloc(size_t size) {
  //try to give from our space,
  void * res = slots1.malloc(size);
  if (res) return res;
  return slots2.malloc(size);
}

void * bin_realloc(void * ptr, size_t size)
{
  if (ptr == 0) {
    //no previous data
    return bin_malloc(size);
  }
  else {
    if (! (slots1.is_member(ptr) || slots2.is_member(ptr)) ) {
      // not our resposibility
      return 0;
    }

    //we have existing data
    // if it fits in current slot, return it
    // TODO if new size is smaller, try to relocate in smaller slot
    if ( slots1.can_fit(ptr, size) ) {
      // TRACE("OUR realloc %p[%lu] fits in slot1", ptr, size); FLUSH();
      return ptr;
    }
    if ( slots2.can_fit(ptr, size) ) {
      // TRACE("OUR realloc %p[%lu] fits in slot2", ptr, size); FLUSH();
      return ptr;
    }

    //we need bigger slot
    void * res = bin_malloc(size);
    if (res == 0) {
      //allocation failure
      // TRACE("bin_malloc [%lu] FAILURE", size); FLUSH();
      res = malloc(size);
      if (res == 0) {
        TRACE("libc malloc [%lu] FAILURE", size); FLUSH();  
        return 0;
      }
    }
    //copy data
    memcpy(res, ptr, slots1.size(ptr) + slots2.size(ptr));
    bin_free(ptr);
    return res;
  }
}


void *bin_l_alloc (void *ud, void *ptr, size_t osize, size_t nsize)
{
  (void)ud; (void)osize;  /* not used */
  if (nsize == 0) {
    if (ptr) {   // avoid a bunch of NULL pointer free calls
      if (!bin_free(ptr)) {
        // not our range, use libc allocator
        // TRACE("libc free %p", ptr); FLUSH();
        free(ptr);
      }
    }
    return NULL;
  }
  else {
    if (SimulateMallocFailure < 0 ) {
      //delayed failure
      if (++SimulateMallocFailure == 0) {
        SimulateMallocFailure = 1;
      }
    }
    if ( SimulateMallocFailure > 0) {
      // simulate one malloc failure
      TRACE("bin_l_alloc(): simulating malloc failure at %p[%lu]", ptr, nsize); FLUSH();
      return 0;
    }
    // if requested size fints in our slots
    // and we have a free slot,
    // then use our allocator
    // else use lic allocator
    void * res = bin_realloc(ptr, nsize);
    if (res && ptr) {
      // TRACE("OUR realloc %p[%lu] -> %p[%lu]", ptr, osize, res, nsize); FLUSH(); 
    }
    if (res == 0) {
      res = realloc(ptr, nsize);
      // TRACE("libc realloc %p[%lu] -> %p[%lu]", ptr, osize, res, nsize); FLUSH();
      if (res == 0 ){
        TRACE("realloc FAILURE %lu", nsize);
        dumpFreeMemory();
      }
    }
    return res;
  }
}
