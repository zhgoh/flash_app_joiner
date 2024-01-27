#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Struct for holding file name and it's pointer
 */
typedef struct
{
  char* name;
  FILE* file;
} FileType;

/*
 * Is actually getting rid of the .swf extension and replacing it
 * with .exe
 */
char* getSwfName(char* name)
{
  char* swfName = name;
  int i = 0;
  
  while (*swfName != '.')
  {
    ++swfName;
    ++i;
  }
  
  *swfName = '\0';
  strcat(swfName, ".exe");    /* Making it .exe instead                       */
  swfName -= i;               /* Remember to return the correct position      */
  return swfName;
}

/*
 * First get the file length in bytes, e.g. 0x12AD
 * and next we reverse it for little endian to 0xAD12 and
 * then feed it into the EOF of the binary output
 */
void GetFileLength(char* buffer, int size)
{
  int i = 0;
  int j = 0;
  int k = 0;
  char tmpA = 0;
  char tmpB = 0;
  char* tmpBuffer = buffer;
  
  sprintf(buffer, "%x", size);
  
  while (*tmpBuffer)
  {
    ++tmpBuffer;
    ++i;
  }
  
  j = i;
  
  if (i % 2)
  {
    while (i > 0)
    {
      buffer[i] = buffer[i - 1];
      --i;
    }
    buffer[0] = '0';
    ++j;
  }
  
  /*
   * Flip the characters
   */
  i = 0;
  while (i < j)
  {
    if ((i * 2) + 1 > j)
    {
      break;
    }
    
    tmpA = buffer[i];
    tmpB = buffer[i + 1];
    buffer[i] = buffer[j - i - 2]; 
    buffer[i + 1] = buffer[j - i - 1];
    buffer[j - i - 2] = tmpA;
    buffer[j - i - 1] = tmpB;
    i += 2;
  }
  
  /*
   * Iterate through the array of even elements, e.g. 6 elements
   * and add the first 2 elements together and put it into the previous 
   * available slot.
   */
  for (i = 0, k = 0; i <= (j / 2); ++i, k += 2)
  {
    tmpA = buffer[k]; 
    tmpB = buffer[k + 1];
    
    if (tmpA >= 'a' && tmpA <= 'f')
    {
      tmpA -= 87;
    }
    else if(tmpA >= '0' && tmpA <= '9')
    {
      tmpA -= 48;
    }
    
    if (tmpB >= 'a' && tmpB <= 'f')
    {
      tmpB -= 87;
    }
    else if(tmpB >= '0' && tmpB <= '9')
    {
      tmpB -= 48;
    }
    
    tmpA = tmpA << 4;
    buffer[i] = (tmpA + tmpB);
  }
  
  /*
   * Only add trailing zeroes to fill up our qword bytes
   */
  if (i < 4)
  {
    buffer[3] = '\0';
  }
}

/*
 * What this program does is, it will take a swf file in and a FlashPlayer.exe
 * and write out to a new file with the same name as our game with .exe instead
 * , and one more thing, after writing both the player and movie into our binary
 * , we got to add this magic number 0xFA123456 and the length of our swf file
 * (both QWORD) into the EOF.
 */
int main (int argc, char* argv[])
{
  FileType flashPlayer;
  FileType flashSwf;
  long int size = 0;
  char* buffer  = NULL;
  FILE* outFile = NULL;
  
  char fileSizeArray[20];
  const unsigned char MAGIC[] = {0x56, 0x34, 0x12, 0xFA};
  
  if (argc < 2)
  {
    printf("Not enough arguments.\n");
    printf("Usage: CreateProjector.exe Game.swf \n");
  }
  else
  {
    /* flashPlayer.name = argv[1];
    flashSwf.name    = argv[2]; */
    flashPlayer.name = "flashplayer_32_sa.exe";
    flashSwf.name    = argv[1];
    
    flashPlayer.file = fopen(flashPlayer.name, "rb");
    flashSwf.file    = fopen(flashSwf.name, "rb");
    outFile          = fopen(getSwfName(flashSwf.name), "wb");
    
    /*
     * Only if both file are valid, then we 
     * start to do stuff
     */
    if (flashPlayer.file && flashSwf.file && outFile)
    {
      /*
       * Fseek returns 0 if it can find SEEK_END
       */
      if (!fseek(flashPlayer.file, 0l, SEEK_END))
      {
        size = ftell(flashPlayer.file);/* This tells us the size of the file */
        rewind(flashPlayer.file);      /* File points back to the start      */
        buffer = malloc(size);
        fread(buffer, 1, size, flashPlayer.file);
        fwrite(buffer, 1, size, outFile);
        free(buffer);
      }
      
      /*
       * Fseek returns 0 if it can find SEEK_END
       */
      if (!fseek(flashSwf.file, 0l, SEEK_END))
      {
        size = ftell(flashSwf.file);/* This tells us the size of the file */
        rewind(flashSwf.file);      /* File points back to the start      */
        buffer = malloc(size);
        fread(buffer, 1, size, flashSwf.file);
        fwrite(buffer, 1, size, outFile);
        free(buffer);
      }
      
      GetFileLength(fileSizeArray, size);
      
      fwrite(MAGIC, 1, 4, outFile);
      fwrite(fileSizeArray, 1, 4, outFile);
    }
   
    fclose(outFile);
    fclose(flashPlayer.file);
    fclose(flashSwf.file);
  }
  return 0;
}