#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

   if(argc != 3)
      return -1;

   char ch, *input_file_name, *output_file_name;
      FILE *input_file, *output_file;

   input_file_name = argv[1];
   output_file_name = argv[2];

   input_file = fopen(input_file_name,"r"); // read mode
   output_file = fopen(output_file_name, "w"); // write mode
 
   if(input_file == NULL || output_file == NULL)
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
   
   while( ( ch = fgetc(input_file) ) != EOF ) {
      int i = ch;
      if(i != 0) {
         fprintf(output_file, "%c", (char)i);
      }
   }
 
   fclose(input_file);
   fclose(output_file);

   return 0;
}