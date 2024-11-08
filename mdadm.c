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


static int mount  = 0;
//int temp[32] = 

uint32_t create_op(uint32_t DiskID, uint32_t BlockID, uint32_t Command, uint32_t Reserved) {
  //DiskID
  uint32_t op = 0; // 00000000 00000000 00000000 00000000
  op = op | Command; // 00000000 00000000 00000000 11111111
  op = op << 8; // 00000000 00000000 11111111 00000000
  op = op | BlockID; // 00000000 00000000 11111111 11111111
  op = op << 4; // 000000000 00001111 11111111 11110000
  op = op | DiskID; // 000000000000 11111111 11111111 1111 : reserved, command, blockID, diskID
  return op;
}
// you mount a file for the file system to have access to it
// a disk is broken up into blocks and each block is the size of 
// total memory is 2^20 bytes


int mdadm_mount(void) {

  if (mount==1){
    return -1; // already mounted 
  } 

  uint32_t op = create_op(0,0,JBOD_MOUNT, 0); // create an operator
  int final = jbod_operation(op, NULL); // create jbod_operation
  if (final == 0){ // if opertion return 0
    mount = 1;
    return 1;
  } else {
    return -1;
  }
}

int mdadm_unmount(void) {
  if (mount == 0){
    return -1;  
  }
  uint32_t op = create_op(0,0,JBOD_UNMOUNT, 0); 
  int final = jbod_operation(op, NULL);
  if (final == 0){ // if operation return 1 
    mount = 0;
    return 1;
  } else{
    return -1;
  }
}




int mdadm_read(uint32_t start_addr, uint32_t read_len, uint8_t *read_buf)  {
  
//  Complete your code here, take from the start_addr for read_len and move to read_buf
 uint32_t Disk = start_addr / JBOD_DISK_SIZE; // starting disk
 uint32_t Block = (start_addr % JBOD_DISK_SIZE) / JBOD_BLOCK_SIZE; // starting block
 uint32_t offset = (start_addr % JBOD_BLOCK_SIZE);
 


 uint32_t bytes_left = read_len;
 uint32_t bytes_read = 0;
 // uint32_t bytes_finished = 0;
 uint32_t curr_addr = start_addr;
//  printf("%u \n", start_addr);
 uint32_t end_addr = start_addr + read_len;
 uint8_t temp_buffer[JBOD_BLOCK_SIZE];


 // edge cases: start address is greater than 1mb, end address is start address + read_len
 if (end_addr > 1048576){ // if address space exceeds 1mb
   return -1;
 }
 if (read_len>1024){ // if read_len exceeds 1024 bytes
   return -2;
 }
 if (mount == 0){ // if system is unmounted
   return -3;
 }

 if (read_buf == NULL && read_len > 0){
   return -4;
 }
//  if (start_addr > 0 || (read_buf == NULL && read_len > 0)){
//    return -4;
//  }

 if (read_len == 0 || read_buf == NULL){
  return 0;
 }


// 000000000000 11111111 11111111 1111
  while (bytes_left>0){
    
   uint32_t Curr_Disk = create_op(Disk, 0, JBOD_SEEK_TO_DISK, 0); // current address
   uint32_t Curr_Block = create_op(0, Block, JBOD_SEEK_TO_BLOCK, 0); // current address
   jbod_operation(Curr_Disk, NULL);
   jbod_operation(Curr_Block, NULL);

   uint32_t read = create_op(Disk, Block, JBOD_READ_BLOCK, 0); // create reading operation
   uint32_t op = jbod_operation(read, temp_buffer); // reads the entire block and puts it in temp buf
   if (op == -1){
     return -1;
   } // increasing block by 1

    if (offset + bytes_left <= JBOD_BLOCK_SIZE){ // in the same block or final block
      memcpy(read_buf + bytes_read, temp_buffer + offset, end_addr - curr_addr);
      return read_len;
    } else { // switching blocks
      memcpy(read_buf + bytes_read, temp_buffer + offset, JBOD_BLOCK_SIZE - offset); // copy offset to end of block
      bytes_left -= JBOD_BLOCK_SIZE - offset;
      bytes_read += JBOD_BLOCK_SIZE - offset;
      printf("%u %u\n", bytes_read, bytes_left);
    }

    curr_addr += (JBOD_BLOCK_SIZE - offset);
    Disk = curr_addr / JBOD_DISK_SIZE; // current disk
    Block = (curr_addr % JBOD_DISK_SIZE) / JBOD_BLOCK_SIZE; // current block
    offset = (curr_addr % JBOD_BLOCK_SIZE); // new offset
    
   }
  return read_len;
}