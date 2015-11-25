#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GetBit(var, bit) ((var & (1 << bit)) != 0)
#define SetBit(var, bit) (var |= (1 << bit))

struct {
  unsigned char a : 8 ;
} Ag ;

void key_generator( char* key, char* k1, char* k2 )
{
  char p [10] = {key[4], key[1], key[6], key[3], key[2], key[0], key[8], key[7], key[5], key[9]} ;
  char p1 [10] = {key[6], key[3], key[2], key[4], key[1], key[7], key[5], key[9], key[0], key[8]} ;
  char p8 [8] = {5, 2, 6, 3, 7, 4, 9, 8} ;
  int i = 0 ;
  
  for (i = 0 ; i < 8; i++)
  {
    k1[i] = p[ p8[i] ] ;
    k2[i] = p1[ p8[i] ] ;
  }

  k1[8] = '\0' ;
  k2[8] = '\0' ;

}

void xor_function( char* vector, char* plaintext, char* result, int size )
{
  int i ;
  
  for (i=0; i < size; i++)
  {
    if ( (vector[i] == '0' && plaintext[i] == '1') || (vector[i] == '1' && plaintext[i] == '0') )
      result[i] = '1' ;
    else
      result[i] = '0' ;
  }
  result[size] = '\0' ;

}

void initial_final_permutations( char* x, char* result, int which )
{
  char ip [8] = {1, 5, 2, 0, 3, 7, 4, 6} ;
  char nip [8] = {3, 0, 2, 4, 6, 1, 7, 5} ;
  int i ;
  
  if (which == 0)
    for (i = 0; i < 8; i++) result[i] = x[ ip[i] ] ;
  else
    for (i = 0; i < 8; i++) result[i] = x[ nip[i] ] ;
    
  result[8] = '\0' ;
}

int to_int( char first, char second )
{
  if (first == '1' && second == '1' )
    return 3 ;
  else if (first == '1' && second == '0')
    return 2 ;
  else if (first == '0' && second == '1')
    return 1 ;
  else
    return 0 ;
}

void to_char( int x, char* result )
{
  if (x == 3)
    strcat( result, "11" ) ;
  else if (x == 2)
    strcat( result, "10" ) ;
  else if (x == 1)
    strcat( result, "01" ) ;
  else
    strcat( result, "00" ) ;
}

void f_function( char* text, char* key, char* result )
{

  char expand [8] = {text[7], text[4], text[5], text[6], text[5], text[6], text[7], text[4]} ;
  int s0 [4][4] = { {1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2} } ;
  int s1 [4][4] = { {0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3} } ;
  char p4 [4] = {1, 3, 2, 0} ;
  char buffer [9] ;
  char buffer2 [4] ;
  int i ;
  int row ;
  int col ;
  
  memset(buffer2, '\0', sizeof(buffer2)) ;
  
  //Add to key
  xor_function( expand, key, buffer, 8 ) ;
  
  //SO
  row = to_int( buffer[0], buffer[3] ) ;
  col = to_int( buffer[1], buffer[2] ) ;
  to_char( s0[row][col], buffer2 ) ;
  
  //S!
  row = to_int( buffer[4], buffer[7] ) ;
  col = to_int( buffer[5], buffer[6] ) ;
  to_char( s1[row][col], buffer2 ) ;
  
  //Permute
  for (i = 0; i < 4; i++)
    buffer[i] = buffer2[ p4[i] ] ;

  //Add to L
  xor_function( text, buffer, buffer2, 4 ) ;
  buffer2[4] = '\0' ;
  
  //Return L
  strcat( result, buffer2 ); 

  //Return R
  for (i = 0; i < 4; i++)
    result[i+4] = text[i+4] ;

}

void switch_function( char* text, char* result )
{
  int i ;
  
  for (i = 0; i < 8; i++) 
    result[i] = text[ (i+4) % 8 ] ;
}

void encrypt( char* k1, char* k2, unsigned char* plaintext, unsigned char* vector, char* result ) 
{
  char buffer1 [9] ;
  char buffer2 [9] ;
  char f_result [9] ;
 
  //Add to vector 
  xor_function( plaintext, vector, buffer1, 8 ) ;
  
  //Initial permutation
  initial_final_permutations( buffer1, buffer2, 0 ) ;
    
  
  //fk1
  memset( f_result, '\0', sizeof(f_result) ) ;
  f_function( buffer2, k1, f_result) ;

  //Switch
  switch_function( f_result, buffer1 ) ;
  
  //fk2
  memset(f_result, '\0', sizeof(f_result)) ;
  f_function( buffer1, k2, f_result ) ;
  
  //Final Permutation
  initial_final_permutations( f_result, result, 1 ) ;   
}

int main( int argc, char* argv[] )
{
  char k1 [9] ;
  char k2 [9] ;
  unsigned char plaintext ;
  char space[1] ;
  unsigned char buffer1 ;
  unsigned char buffer2 ;
  int bytes_read ;
  FILE* input_file ;
  FILE* output_file ;
  
  buffer1 = strtol( argv[1], 0, 2 ) ;
  buffer2 = 'a' ;
  
  buffer1 = buffer1 ^ buffer2 ;
  int i ;
  for (i=7; i>=0; i--)
  {
    if (GetBit(buffer1,i))
      printf("1") ;
    else
      printf("0") ;  
  }
  printf("\n") ;
  
  /*
  //Generate Keys
  key_generator( argv[1], k1, k2 ) ;
  
  //open files
  input_file = fopen( argv[3], "rb" ) ;
  output_file = fopen( argv[4], "wb" ) ;  
  
  //check files
  if(input_file == NULL || output_file == NULL)
  {
    printf("Error opening file\n") ;
    return -1 ;
  }
  
  //read first byte
  bytes_read = fread( &plaintext, 1, 1, input_file ) ;  

  buffer1 = strtol( argv[2], 0, 2 ) ;
  
  while( bytes_read > 0 )
  {
    encrypt( k1, k2, plaintext, buffer1, buffer2 ) ;
    //fwrite( buffer2, 1, 8, output_file ) ;
    
    memset( plaintext, '\0', sizeof(plaintext) ) ;
    bytes_read = fread( plaintext, 1, 1, input_file ) ;
    strcpy( buffer1, buffer2 ) ;    
  }
  */
  /*
  int i ;
  for (i=7; i>=0; i--)
  {
    if (GetBit(plaintext,i))
      printf("1") ;
    else
      printf("0") ;  
  }
  printf("\n") ;*/
  
  
}
