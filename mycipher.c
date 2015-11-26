#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GetBit(var, bit) ((var & (1 << bit)) != 0)
#define SetBit(var, bit) (var |= (1 << bit))

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

unsigned int permutations( unsigned char x, int which )
{
  int ip [8] = {1, 3, 0, 4, 7, 5, 2, 6} ;
  int nip [8] = {2, 0, 6, 1, 3, 5, 7, 4} ;
  int e_p [8] = {3, 0, 1, 2, 1, 2, 3, 0} ;
  int p4 [4] = {7, 5, 4, 6} ;
  unsigned char result = 0 ;
  int i ;
  
  if (which == 0)
  {
    for (i = 7; i >= 0 ; i--)
    {
      if (GetBit(x,ip[i]))
        SetBit(result,i);
    }
  }
  else if (which == 1)
  {
    for (i = 7; i >= 0 ; i--)
    {
      if (GetBit( x,nip[i] ))
        SetBit( result,i );
    }
  }
  else if (which == 2)
  {
    for (i = 7; i >= 0; i--)
    {
      if (GetBit( x,e_p[i] ))
        SetBit( result,i );
    }
  }
  else if (which == 3)
  {
    for (i = 3; i >= 0; i--)
    {
      if (GetBit( x,p4[i] ))
        SetBit( result,i+4 );
    }
  }
    
  return result ;
}

unsigned char s_box( int r0, int c0, int r1, int c1 )
{
  int s0 [4][4] = { {1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2} } ;
  int s1 [4][4] = { {0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3} } ;
  unsigned char result = 0 ;
  
  if (GetBit( s0[r0][c0],1 ))
    SetBit( result, 7 ) ;
  if (GetBit( s0[r0][c0],0 ))
    SetBit( result, 6 ) ;
    
  if (GetBit( s1[r1][c1],1 ))
    SetBit( result, 5 ) ;
  if (GetBit( s1[r1][c1],0 ))
    SetBit( result, 4 ) ;
  
  return result ;
}

unsigned char f_function( unsigned char text, unsigned char key )
{
  unsigned char buffer1 ;
  unsigned char buffer2 ;
  unsigned char result = 0 ;
  int i ;
  int row = 0;
  int row1 = 0;
  int col = 0;
  int col1 = 0 ;
  
  //Permute P8
  buffer1 = permutations( text, 2 ) ;
  
  //Add to key
  buffer2 = buffer1 ^ key ;

  //SO
  if (GetBit( buffer2, 7 ))
    SetBit( row, 1 ) ;
  if (GetBit( buffer2, 4 ))
    SetBit( row, 0 ) ;    
  
  if (GetBit( buffer2, 6 ))
    SetBit( col, 1 ) ;
  if (GetBit( buffer2, 5 ))
    SetBit( col, 0 ) ; 
    
  //S!  
  if (GetBit( buffer2, 3 ))
    SetBit( row1, 1 ) ;
  if (GetBit( buffer2, 0 ))
    SetBit( row1, 0 ) ;    
  
  if (GetBit( buffer2, 2 ))
    SetBit( col1, 1 ) ;
  if (GetBit( buffer2, 1 ))
    SetBit( col1, 0 ) ;

  //Input to boxes
  buffer1 = s_box( row, col, row1, col1 ) ;
  
  //Permute P4
  buffer2 = permutations( buffer1, 3 ) ;

  //Add to L and append right
  result = text ^ buffer2 ;
  
  return result ;
}

unsigned char switch_function( unsigned char text )
{
  int i ;
  unsigned char result = 0 ;
  
  for (i = 7; i >= 0; i--)
  {
    if (GetBit( text, i ))
      SetBit( result, (i+4) % 8 );
  } 
  return result ;
}

unsigned char encrypt( unsigned char k1, unsigned char k2, unsigned char plaintext, unsigned char vector ) 
{
  unsigned char buffer1 ;
  unsigned char buffer2 ;
  
  //Add to vector 
  buffer1 = plaintext ^ vector ;

  //Initial permutation
  buffer2 = permutations( buffer1, 0 ) ;  

  //fk1
  buffer1 = f_function( buffer2, k1 ) ;

  //Switch
  buffer2 = switch_function( buffer1 ) ;
  
  //fk2
  buffer1 = f_function( buffer2, k2 ) ;
  
  //Final Permutation
  return permutations( buffer1, 1 ) ;   
}

unsigned char decrypt( unsigned char k1, unsigned char k2, unsigned char ciphertext, unsigned char vector )
{
  unsigned char buffer1 ;
  unsigned char buffer2 ;
  
  //Initial permutation
  buffer2 = permutations( ciphertext, 0 ) ;
  
  //fk2
  buffer1 = f_function( buffer2, k2 ) ;
  
  //Switch
  buffer2 = switch_function( buffer1 ) ;
  
  //fk1
  buffer1 = f_function( buffer2, k1 ) ;
  
  //Final Permutation
  buffer2 = permutations( buffer1, 1 ) ;
  
  //Add to vector 
  return buffer2 ^ vector ;
}

int main( int argc, char* argv[] )
{
  char key1 [8] ;
  char key2 [8] ;
  unsigned char k1 ;
  unsigned char k2 ;
  unsigned char plaintext ;
  unsigned char ciphertext ;
  unsigned char buffer1 ;
  unsigned char buffer2 ;
  FILE* input_file ;
  FILE* output_file ;
  
  if (argc == 6 )
  {
    //Generate Keys
    key_generator( argv[2], key1, key2 ) ;
    k1 = strtol( key1, 0, 2 ) ;
    k2 = strtol( key2, 0, 2 ) ;
    
    //open files
    input_file = fopen( argv[4], "rb" ) ;
    output_file = fopen( argv[5], "wb" ) ;  
    
    //check files
    if(input_file == NULL || output_file == NULL)
    {
      if( input_file == NULL )
        printf("Error opening input file\n") ;
      return -1 ;
    }
    
    //read first byte
    fread( &ciphertext, 1, 1, input_file ) ;  

    buffer1 = strtol( argv[3], 0, 2 ) ;
    
    while( !feof(input_file) )
    {
      buffer2 = decrypt( k1, k2, ciphertext, buffer1 ) ;

      fwrite( &buffer2, 1, 1, output_file ) ;
      
      buffer1 = ciphertext ; 
      
      fread( &ciphertext, 1, 1, input_file ) ;   
    }
    fclose( input_file ) ;
    fclose( output_file ) ; 
  } 
  else
  {
    //Generate Keys
    key_generator( argv[1], key1, key2 ) ;
    k1 = strtol( key1, 0, 2 ) ;
    k2 = strtol( key2, 0, 2 ) ;
    
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
    fread( &plaintext, 1, 1, input_file ) ;  

    buffer1 = strtol( argv[2], 0, 2 ) ;
    
    while( !feof(input_file) )
    {
      buffer2 = encrypt( k1, k2, plaintext, buffer1 ) ;

      fwrite( &buffer2, 1, 1, output_file ) ;
      
      fread( &plaintext, 1, 1, input_file ) ;

      buffer1 = buffer2 ;   
    }
    fclose( input_file ) ;
    fclose( output_file ) ;
  }
  
}
