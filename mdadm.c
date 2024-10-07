#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mdadm.h"
#include "jbod.h"

//disk size == address
// create operation
// add == block size
// edge cases to worry about: you can only read 4 blocks at a time
// if you are inside the block of a block, you want to make sure that you are not in a differnt block


static int mounted_status = 0;
//int temp[32] = 

uint32_t create_op(uint32_t DiskID, uint32_t BlockID, uint32_t Command, uint32_t Reserved) {
  //DiskID
  uint32_t op = 0; // op = 0
  op = op | Command; // op = 1111
  op = op << 8; // 1111 00000000
  op = op | BlockID; // 1111 11111111
  op = op << 8; // 1111 11111111 00000000
  op = op | DiskID; // 1111 11111111 11111111  
  op = op << 12; // 1111 11111111 11111111 000000000000 
  return op;
}
// you mount a file for the file system to have access to it
// a disk is broken up into blocks and each block is the size of 256
// total memory is 2^20 bytes


int mdadm_mount(void) {

  if (mounted_status == 1){
    return -1;
  } 
  // use the 
  return 0;
}

int mdadm_unmount(void) {
  //Complete your code here
  return 0;

}

int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)  {
  //Complete your code here, take from the start_addr for read_len and move to read_buf
  // edge cases are in pdf
  return 0;
}