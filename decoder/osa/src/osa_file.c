

#include <osa_file.h>


int OSA_fileReadFile(char *fileName, Uint8 *addr, Uint32 readSize, Uint32 *actualReadSize)
{
  int retVal = OSA_SOK;
  Uint8  *curAddr;

  Uint32 readDataSize, fileSize, chunkSize=1024*100;
	Uint32 userReadSize;

  FILE *hndlFile;

  #ifdef OSA_DEBUG_FILE
  OSA_printf(" [OSA_FILE ] Reading file [%s] ... ", fileName);
  #endif

  hndlFile = fopen(fileName, "rb");

  if(hndlFile == NULL) {
  	retVal = OSA_EFAIL;
    goto exit;
	}

  curAddr = addr;
  fileSize = 0;

	userReadSize = readSize;

  while(1) {
		if(userReadSize != 0) {
			if(chunkSize > userReadSize)
				chunkSize = userReadSize;
			readDataSize = fread(curAddr, 1, chunkSize, hndlFile);
			fileSize += readDataSize;
			if(chunkSize != readDataSize)
				break;
			if(userReadSize==fileSize)
				break;
			curAddr += chunkSize;
		}
		else {
			readDataSize = fread(curAddr, 1, chunkSize, hndlFile);
      fileSize+=readDataSize;
      if(chunkSize!=readDataSize)
        break;
      curAddr+=chunkSize;
    }
	}
  #ifdef OSA_DEBUG_FILE
  OSA_printf("Done. [%d bytes] \r\n", fileSize);
  #endif
  fclose(hndlFile);

exit:
  if(retVal!=OSA_SOK) {
    #ifdef OSA_DEBUG_FILE
    OSA_printf("ERROR \r\n");
    #endif
    fileSize=0;
  }
	if(actualReadSize != NULL)
    *actualReadSize = fileSize;

  return retVal;
}

int OSA_fileWriteFile(char *fileName, Uint8 *addr, Uint32 size)
{
  int retVal = OSA_SOK;
  Uint32 writeDataSize;

 // Bool errorInWriting = FALSE;

  FILE *hndlFile;

	if(size==0)
		return OSA_SOK;

  #ifdef OSA_DEBUG_FILE
  OSA_printf(" [FILE ] Writing to file [%s] (%d bytes) ... ", fileName, size);
  #endif
  hndlFile = fopen(fileName, "wb");

  if(hndlFile == NULL) {
  	retVal = OSA_EFAIL;
    goto exit;
	}

  {
    // write in units of chunkSize
    Int32 fileSize, chunkSize = 96*1024;
    Int8  *curAddr;


    fileSize = size;
    curAddr  = (Int8*)addr;
    while(fileSize>0) {
      if(fileSize<chunkSize) {
        chunkSize = fileSize;
      }
      writeDataSize=0;
      writeDataSize = fwrite(curAddr, 1, chunkSize, hndlFile);
      if(writeDataSize!=chunkSize) {
        // error in writing, abort
       // errorInWriting = TRUE;
        retVal = OSA_EFAIL;
        break;
      }
      curAddr += chunkSize;
      fileSize -= chunkSize;
    }
    writeDataSize = size - fileSize;
  }

  #ifdef OSA_DEBUG_FILE
  OSA_printf("Done. [%d bytes] \r\n", writeDataSize);
  #endif
  fflush(hndlFile);

  fclose(hndlFile);

exit:
  if(retVal!=OSA_SOK) {
    #ifdef OSA_DEBUG_FILE
    OSA_printf("ERROR \r\n");
    #endif
  }
  return retVal;

}

int OSA_fileWriteVideoData(char *fileName, Uint8 *Yaddr, Uint8 *UVaddr, Uint32 width, Uint32 height, Uint32 pitch)
{
  int retVal = OSA_SOK;
  Uint32 writeDataSize;
  FILE *hndlFile;
  int writesize=(width*height*3)>>1;

  if((width==0)||(height==0)||(pitch==0))
		return OSA_SOK;

  #ifdef OSA_DEBUG_FILE
  OSA_printf(" [FILE ] Writing to file [%s] (%d bytes) ... ", fileName, writesize);
  #endif

  hndlFile = fopen(fileName, "wb");
  if(hndlFile == NULL) {
  	retVal = OSA_EFAIL;
    goto filewrite_abort;
  }

  {
    // write Y data as per pitch
    Int32 fileSize = width*height, chunkSize = width;
    Int8  *curAddr = (Int8*)Yaddr;

    while(fileSize>0) {
      writeDataSize = fwrite(curAddr, 1, chunkSize, hndlFile);
      if(writeDataSize!=chunkSize) {
        // error in writing, abort
        retVal = OSA_EFAIL;
    	goto filewrite_abort;
      }
      curAddr += pitch;
      fileSize -= chunkSize;
    }
  }

  {
    // write UV data as per pitch
    Int32 fileSize = (width*height)>>1, chunkSize = width;
    Int8  *curAddr = (Int8*)UVaddr;

    while(fileSize>0) {
      writeDataSize = fwrite(curAddr, 1, chunkSize, hndlFile);
      if(writeDataSize!=chunkSize) {
        // error in writing, abort
        retVal = OSA_EFAIL;
    	goto filewrite_abort;
      }
      curAddr += pitch;
      fileSize -= chunkSize;
    }
  }

  #ifdef OSA_DEBUG_FILE
  OSA_printf("Done. [%d bytes] \r\n", writesize);
  #endif
  fflush(hndlFile);

  fclose(hndlFile);

filewrite_abort:
  if(retVal!=OSA_SOK) {
    #ifdef OSA_DEBUG_FILE
    OSA_printf("ERROR \r\n");
    #endif
  }
  return retVal;

}

