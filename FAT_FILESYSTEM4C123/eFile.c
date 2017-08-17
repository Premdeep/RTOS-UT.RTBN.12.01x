// eFile.c
// Runs on either TM4C123 or MSP432
// High-level implementation of the file system implementation.
// Daniel and Jonathan Valvano
// August 29, 2016
#include <stdint.h>
#include "eDisk.h"

uint8_t Buff[512]; // temporary buffer used during file I/O
uint8_t Directory[256], FAT[256];
int32_t bDirectoryLoaded =0; // 0 means disk on ROM is complete, 1 means RAM version active

// Return the larger of two integers.
int16_t max(int16_t a, int16_t b){
  if(a > b){
    return a;
  }
  return b;
}
//*****MountDirectory******
// if directory and FAT are not loaded in RAM,
// bring it into RAM from disk
void MountDirectory(void){ 
// if bDirectoryLoaded is 0, 
//    read disk sector 255 and populate Directory and FAT
//    set bDirectoryLoaded=1
// if bDirectoryLoaded is 1, simply return
// **write this function**
	if(bDirectoryLoaded == 0){
		
		uint16_t i = 0;
		while(i < 256){
			Directory[i] = 255;
			FAT[i] = 255;
			i++;
		}
		bDirectoryLoaded = 1;
		
	}
	return;  
	
}

// Return the index of the last sector in the file
// associated with a given starting sector.
// Note: This function will loop forever without returning
// if the file has no end (i.e. the FAT is corrupted).
uint8_t lastsector(uint8_t fsNumber){
// **write this function**
	if(fsNumber == 255)
		return 255;
	else{
		//uint8_t m = FAT[fsNumber];
		while( FAT[fsNumber] != 255)				
			fsNumber = FAT[fsNumber];		
	}  
	
  return fsNumber; // replace this line
}

// Return the index of the first free sector.
// Note: This function will loop forever without returning
// if a file has no end or if (Directory[255] != 255)
// (i.e. the FAT is corrupted).
uint8_t findfreesector(void){
// **write this function**
	int16_t fsi = -1;
	uint8_t i = 0;
	uint8_t ls;
	
	while( i < 255){
	ls = lastsector(Directory[i]);
	if(ls == 255)
		return fsi+1;
	else{
		fsi = max(fsi, ls);
		i++;
	}
}
	
  
  return i; // replace this line
}

// Append a sector index 'n' at the end of file 'num'.
// This helper function is part of OS_File_Append(), which
// should have already verified that there is free space,
// so it always returns 0 (successful).
// Note: This function will loop forever without returning
// if the file has no end (i.e. the FAT is corrupted).
uint8_t appendfat(uint8_t num, uint8_t freeSectorNum){
// **write this function**
	uint8_t i = Directory[num];
	if(i == 255){ // brand new file && 1st file
		Directory[num] = freeSectorNum;
	}else{
		while(FAT[i] != 255){
			i = FAT[i];
		}
		FAT[i] = freeSectorNum;
 	}
  
	
  return 0; // replace this line
}

//********OS_File_New*************
// Returns a file number of a new file for writing
// Inputs: none
// Outputs: number of a new file
// Errors: return 255 on failure or disk full
uint8_t OS_File_New(void){
// **write this function**
// returns the file number which is just the index of the directory.
	MountDirectory();
	uint8_t i = 0;
	while(i < 255){
		if(Directory[i] == 255)
			return i;
		i++;
	}	
  return 255;
}

//********OS_File_Size*************
// Check the size of this file
// Inputs:  num, 8-bit file number, 0 to 254
// Outputs: 0 if empty, otherwise the number of sectors
// Errors:  none
uint8_t OS_File_Size(uint8_t num){
// **write this function**
	uint8_t count = 0;
	if(Directory[num] == 255);		
	else{
		count++;
		uint8_t i = Directory[num];	
		while(FAT[i] != 255){
			i = FAT[i];
			count++;
		}
		
	}
  
	
  return count; // replace this line
}

//********OS_File_Append*************
// Save 512 bytes into the file
// Inputs:  num, 8-bit file number, 0 to 254
//          buf, pointer to 512 bytes of data
// Outputs: 0 if successful
// Errors:  255 on failure or disk full
uint8_t OS_File_Append(uint8_t num, uint8_t buf[512]){
	
	
// **write this function**
	uint8_t fsi = findfreesector();
	if(fsi == 255)
		return fsi;
	eDisk_WriteSector(buf, fsi);
	appendfat(num,fsi);
	return 0;  
  
}

//********OS_File_Read*************
// Read 512 bytes from the file
// Inputs:  num, 8-bit file number, 0 to 254
//          location, logical address, 0 to 254
//          buf, pointer to 512 empty spaces in RAM
// Outputs: 0 if successful

// Errors:  255 on failure because no data
uint8_t OS_File_Read(uint8_t num, uint8_t location,
                     uint8_t buf[512]){
// **write this function**
	uint8_t abs = Directory[num];
	uint8_t i = 0;
	//uint8_t index = abs;
	while(i < location){
		abs = FAT[abs];
		i++;
	}									 
	enum DRESULT result;
	
	if(abs == 255)
		 return 255;
	else
			result = eDisk_ReadSector(buf, abs);

	if(result == RES_OK)
		return 0;
	return 255;
		
											 
  
 // return 0; // replace this line
}

//********OS_File_Flush*************
// Update working buffers onto the disk
// Power can be removed after calling flush
// Inputs:  none
// Outputs: 0 if success
// Errors:  255 one disk write failure
uint8_t OS_File_Flush(void){
// **write this function**
	uint8_t temp[512];
	uint16_t i = 0;
	for( ; i < 256; i++){
		temp[i] = Directory[i];
	}
	for( ; i < 512; i++)
		temp[i] = FAT[i-256];

	eDisk_WriteSector(temp, 255);
	
  return 0; // replace this line
}

//********OS_File_Format*************
// Erase all files and all data
// Inputs:  none
// Outputs: 0 if success
// Errors:  255 on disk write failure
uint8_t OS_File_Format(void){
// call eDiskFormat
// clear bDirectoryLoaded to zero
// **write this function**
	eDisk_Format();
	bDirectoryLoaded = 0;

  return 0; // replace this line
}
