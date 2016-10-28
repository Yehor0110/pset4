/*
* Yehor Valentiukevych
*
* recover.c
*
* Recovers JPEGs from a forensic image.
* 
* check50 =>> https://sandbox.cs50.net/checks/91990ba7106a40a69a3eac459594ab9a
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // for false

#define BLOCK 512

int main(int argc, char* argv[]) 
{
    if (argc != 2)
    {
        fprintf (stderr, "Usage: ./recover img \n"); 
        return 1;
    }
    
    // the file you want to restore
    FILE* file = fopen(argv[1], "r"); 
    if (file == NULL)  
    {
        printf("Error..");
        return 2;
    }
    
    FILE* output ; 
    
    char name[8];
    
    //данные с карты
    unsigned char temp[BLOCK];
    
    //если файл уже открыт
    int open = false;
    int count = 0;
    
    //успешно считанные файлы; 
    while (fread(temp, 1, BLOCK, file) != 0)
    {   
        // проверить, если temp == JPEG           
        if (temp[0] == 0xff && temp[1] == 0xd8 && temp[2] == 0xff && (temp[3] & 0xf0 ) == 0xe0) 
        {   
            //если файл открыт, закрыть.
            if (open)     
                fclose(output );
            
            // Open output file
            sprintf(name, "%03d.jpg", count++ );  
                output = fopen(name, "w");
            open = true;
                fwrite(temp, 1, BLOCK, output );
        } 
        else
        {
            if (open)
                fwrite(temp, 1, BLOCK, output );
        }
    }
    fclose(file);
   fclose(output);
   
  return 0;
}