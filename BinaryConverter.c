//-----------------------------------------------------------------------------
//
// Binary Converter
// 
// Binary numbers get sorted and converted into octal numbers, 
// hexadecimal numbers and decimal numbers
//
// Authors:   Ismail Geles
//            Julia Maier
//          
// Latest Changes: 09.11.2017
//-----------------------------------------------------------------------------
//

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <math.h>

// forward declarations
int eof(void);
int getNumberOctets(void);
int getOctets(int int_octet[CHAR_MAX], int number_octets);
char getEndianOrder(void);
void sort(char endianess, int number_octets, int int_octet[CHAR_MAX],
          int sort_octet[CHAR_MAX]);
void binaryOut(int number_octets, int sort_octet[CHAR_MAX]);
void intArrayToString(int number_octets, int sort_octet[CHAR_MAX],
                      char binary_number[CHAR_MAX]);
int binaryToDecimal(int number_octets, char binary_number[CHAR_MAX]);
int binaryToOctal(int octal[CHAR_MAX], char binary_number[CHAR_MAX]);
int binaryToHexadecimal(int hexadecimal[CHAR_MAX],
                        char binary_number[CHAR_MAX]);
void octalOut(int size_of_octal_array, int octal[CHAR_MAX]);
void decimalOut(int number_octets, unsigned long long decimal_number);
void hexadecimalOut(int size_of_hex_array, int hexadecimal[CHAR_MAX]);

//-----------------------------------------------------------------------------
///
/// The main program.
/// Reads the number of octets, the entered octets and the endianess from stdin
/// and prints the binary number, the octal number, the decimal number and 
/// the hexadecimal number.
/// Before the binary number gets converted it gets sorted into little endian
/// order if neccessary. 
/// 
/// If statements are checking for EOF input.
///
/// @return always zero
//
int main(void)
{
  int number_octets;
  char endianess;
  int int_octet[CHAR_MAX];
  int sort_octet[CHAR_MAX];
  char binary_number[CHAR_MAX];
  unsigned long long decimal_number;
  int octal[CHAR_MAX];
  int size_of_octal_array;
  int hexadecimal[CHAR_MAX];
  int size_of_hex_array;

  // Input for the number of octets the user wants to enter
  if((number_octets = getNumberOctets()) == 0)
  {
    return 0;
  }

  // Input for the octets
  if(getOctets(int_octet, number_octets) == 0)
  {
    return 0;
  }

  // Input to clarify if the octets are in Big or Little Endian order
  if((endianess = getEndianOrder()) == 0)
  {
    return 0;
  }  

  // Octets get sorted if necessary
  sort(endianess, number_octets, int_octet, sort_octet);
  
  printf("binary: ");
  binaryOut(number_octets, sort_octet);

  intArrayToString(number_octets, sort_octet, binary_number);

  decimal_number = binaryToDecimal(number_octets, binary_number);

  size_of_octal_array = binaryToOctal(octal, binary_number);

  size_of_hex_array = binaryToHexadecimal(hexadecimal, binary_number);

  printf("octal: ");
  octalOut(size_of_octal_array, octal);
  
  printf("decimal: ");
  decimalOut(number_octets, decimal_number);

  printf("hexadecimal: ");
  hexadecimalOut(size_of_hex_array, hexadecimal);

  return 0;
}

//-----------------------------------------------------------------------------
///
/// Quits the programm if an EOF (Ctrl + D) occures.
//
int eof(void)
{
  int c;
  if((c = getchar()) == EOF)
  {
    return -1;  
  }
  else
  {
    return 0;
  } 
}

//-----------------------------------------------------------------------------
///
/// Gets the number of octets from stdin and returns this value. 
/// Number of octets must be between 1 and CHAR_MAX.
///
/// @return zero If an EOF occured the return value is 0.
/// @return number_octets The amount of octets the user wants to enter.
//
int getNumberOctets(void)
{
  int number_octets;

  do
  {
    printf("Please enter the number of octets: ");
    scanf("%d", &number_octets);
    if(eof() == -1)
    {
      return 0;
    }

    if(number_octets < 1 || number_octets > CHAR_MAX)
    {
      printf("[ERR] Invalid number of octets.\n");
    }
  }
  while(number_octets < 1 || number_octets > CHAR_MAX);

  return number_octets;
}

//-----------------------------------------------------------------------------
///
/// Gets the octets from stdin.
/// Checks if oktets only have 8 digits containing only 0s and 1s.
///
/// @param int_octet[CHAR_MAX]
/// @param number_octets Amount of octets which get entered.
///
/// @return zero If an EOF occured the return value is 0.
/// @return success If the function is finished without an EOF, it returns 1.
//
int getOctets(int int_octet[CHAR_MAX], int number_octets)
{
  char octet[CHAR_MAX];
  int count_octets;
  int success = 1;

  for(count_octets = 0; count_octets < number_octets; count_octets++) 
  {
    do
    {
      printf("Please enter octet: ");
      scanf("%s", &octet[count_octets]);
      if(eof() == -1)
      {
        return 0;
      }

      sscanf(&octet[count_octets], "%d", &int_octet[count_octets]);
      
       if(strspn(&octet[count_octets], "01") != 8 ||
          strlen(&octet[count_octets]) != 8)
       {
          printf("[ERR] Invalid octet.\n");
       }
    }
    while(strspn(&octet[count_octets], "01") != 8 ||
          strlen(&octet[count_octets]) != 8);
  }
  return success;
}

//-----------------------------------------------------------------------------
///
/// Gets a character from stdin.
/// If the caracter is 'B' the octets are in Big Endian order and if the 
/// character is 'L' the octets are in Little Endian order.
///
/// @return zero If an EOF occured the return value is 0.
/// @return endianess[element] To clarify in which endianess the octets
///                            are entered.
//
char getEndianOrder(void)
{
  char endianess[CHAR_MAX];
  int element = 0;

  do
  {
    printf("Please enter (B)ig Endian or (L)ittle Endian: ");
    scanf("%s", &endianess[element]);
    if(eof() == -1)
    {
      return 0;
    }
  }
  while(((endianess[element] != 'B' && endianess[element] != 'L') ||
          strlen(&endianess[element]) != 1));

  return endianess[element];
}

//-----------------------------------------------------------------------------
///
/// If endianess is 'L' the order of the octets will not be changed, the values 
/// from int_octet get in the same order into sort_octet.
/// If endianess is 'B' the octets in int_octet get sorted into little endian
/// order and then into sort_octet.
/// Therefore one counter can count from the begining for the new order and the 
/// other from the rear.
///
/// @param endianess
/// @param number_octets
/// @param int_octet[CHAR_MAX]
/// @param sort_octet[CHAR_MAX]
//
void sort(char endianess, int number_octets, int int_octet[CHAR_MAX],
          int sort_octet[CHAR_MAX])
{
  int count_octets;
  int count_reverse;

  // Values from last element of int_octet go to first element of sort_octet
  if(endianess == 'B')
  {
    for(count_reverse = number_octets - 1, count_octets = 0;
        count_reverse >= 0; count_reverse--, count_octets++)
    {
      sort_octet[count_octets] = int_octet[count_reverse];
    }
  }
  else if(endianess == 'L')
  {
    for(count_octets = 0; count_octets < number_octets; count_octets++)
    { 
      sort_octet[count_octets] = int_octet[count_octets];
    }
  }
}

//-----------------------------------------------------------------------------
///
/// Prints the binary number.
///
/// @param number_octets
/// @param sort_octet[CHAR_MAX]
//
void binaryOut(int number_octets, int sort_octet[CHAR_MAX])
{
  int count_octets;

  printf("0b");
  for(count_octets = 0; count_octets < number_octets; count_octets++)
  {
    printf(" %08d", sort_octet[count_octets]);
  }
  printf("\n"); 
}

//-----------------------------------------------------------------------------
///
/// Converts an integer array of bytes to one string.
///
/// Bytes have the length of 8, so we need to to add 8 to the counter 
/// from the string.
///
/// @param sort_octet[CHAR_MAX]
/// @param binary_number[CHAR_MAX]
//
void intArrayToString(int number_octets, int sort_octet[CHAR_MAX],
                      char binary_number[CHAR_MAX])
{
  int count_octets;
  int count_figures;
  for(count_octets = 0, count_figures = 0; count_octets < number_octets;
      count_octets++)
  {
    sprintf(&binary_number[count_figures], "%08d", sort_octet[count_octets]);
    count_figures += 8;
  }
}

//-----------------------------------------------------------------------------
///
/// Converts the binary number to a decimal number.
///
/// @param number_octets
/// @param binary_number[CHAR_MAX]
///
/// @return decimal_number
//
int binaryToDecimal(int number_octets, char binary_number[CHAR_MAX])
{
  unsigned long long decimal_number = 0;
  int count_reverse;
  int count_exponent;
  
  // Starts at 8 * number octets - 1, to have the last element of string
  for(count_reverse = (8 * number_octets - 1), count_exponent = 0;
      count_reverse >= 0; count_reverse--, count_exponent++)
  {
    decimal_number += (binary_number[count_reverse] - '0')
      * pow(2, count_exponent);
  }

  return decimal_number;
}

//-----------------------------------------------------------------------------
///
/// Converts the binary number to a octal number.
///
/// The octal number is saved to an string, but first we need to know how big 
/// the octal string is (by length of binary number % 3) and if there remains 
/// one or two (digits), the whole string is added to another which contains 
/// one zero or two zeros. If nothing remains, we don't need to change anything.
/// After that we have always three numbers to change from binary to octal.
///
/// @param octal[CHAR_MAX]
/// @param binary_number[CHAR_MAX]
///
/// @return size_of_octal_array Value to make the output of the octals easier.
//
int binaryToOctal(int octal[CHAR_MAX], char binary_number[CHAR_MAX])
{
  int remainder;
  int size_of_octal_array;
  char two_zerobin[CHAR_MAX] = {"00"};
  char one_zerobin[CHAR_MAX] = {"0"};
  int count_octal;
  int count_figures;
  
  remainder = (strlen(binary_number)) % 3;

  if(remainder == 0)
  {
    size_of_octal_array = (strlen(binary_number)) / 3;
    for(count_octal = 0, count_figures = 0; count_octal < size_of_octal_array;
        count_octal++)
    {
      octal[count_octal] = (binary_number[count_figures] - '0') * pow(2, 2) +
      (binary_number[(count_figures + 1)] - '0') * pow(2, 1) +
      (binary_number[(count_figures + 2)]- '0') * pow(2, 0);
      count_figures += 3;
    }
  }
  else if(remainder == 1)
  {
    strcat(two_zerobin, binary_number);
    size_of_octal_array = (strlen(two_zerobin)) / 3;
    for(count_octal = 0, count_figures = 0; count_octal < size_of_octal_array;
        count_octal++)
    {    
      octal[count_octal] = (two_zerobin[count_figures] - '0') * pow(2, 2) +
      (two_zerobin[(count_figures + 1)] - '0') * pow(2, 1) +
      (two_zerobin[(count_figures + 2)] - '0') * pow(2, 0);
      count_figures += 3;
    }
  }
  else if(remainder == 2)
  {
    strcat(one_zerobin, binary_number);
    size_of_octal_array = (strlen(one_zerobin)) / 3;
    for(count_octal = 0, count_figures = 0; count_octal < size_of_octal_array;
        count_octal++)
    {    
      octal[count_octal] = (one_zerobin[count_figures] - '0') * pow(2, 2) +
      (one_zerobin[(count_figures + 1)] - '0') * pow(2, 1) +
      (one_zerobin[(count_figures + 2)] - '0') * pow(2, 0);
      count_figures += 3;
    }
  }
  return size_of_octal_array;
}

//-----------------------------------------------------------------------------
///
/// Converts the binary number to a hexadecimal number.
///
/// Divides length of binary number with four, so we know how big the array 
/// needs to be (or where the counter stops).
/// Calculates four digits of the string and saves them to the array of 
/// hexadecimals.
///
/// @param hexadecimal[CHAR_MAX]
/// @param binary_number[CHAR_MAX]
///
/// @return size_of_hex_array Value to make the output easier.
//
int binaryToHexadecimal(int hexadecimal[CHAR_MAX],
                        char binary_number[CHAR_MAX])
{
  int size_of_hex_array;
  int count_hex;
  int count_figures;

  size_of_hex_array = (strlen(binary_number) / 4);

  for(count_hex = 0, count_figures = 0; count_hex < size_of_hex_array;
      count_hex++)
  {    
    hexadecimal[count_hex] = (binary_number[count_figures] - '0') * pow(2, 3) +
    (binary_number[count_figures + 1] - '0') * pow(2, 2) +
    (binary_number[(count_figures + 2)] - '0') * pow(2, 1) +
    (binary_number[(count_figures + 3)] - '0') * pow(2, 0);
    // Plus 4 for going with next four digits.
    count_figures += 4;
  }
  return size_of_hex_array;
}

//-----------------------------------------------------------------------------
///
/// Prints the octal number.
///
/// @param size_of_octal_array
/// @param octal[CHAR_MAX]
//
void octalOut(int size_of_octal_array, int octal[CHAR_MAX])
{
  int count_octal;
  int count_zero;

  // Shows where there are no leading zeros anymore. 
  for(count_zero = 0; ;)
  {
    if(octal[count_zero] == 0)
    {
      count_zero++;
    }
    else if(octal[count_zero] != 0)
    {
      break;
    }
  }
  // Starts where after the leading zeros.
  printf("0");
  for(count_octal = count_zero; count_octal < size_of_octal_array;
      count_octal++)
  {
    printf("%d", octal[count_octal]);
  }
  printf("\n");
}

//-----------------------------------------------------------------------------
///
/// Prints the decimal number.
///
/// @param number_octets
/// @param decimal_number
//
void decimalOut(int number_octets, unsigned long long decimal_number)
{
  if(number_octets > 8)
  {
    printf("[OV] max 8 Bytes.\n");
  }
  else
  {
    printf("%llu\n", decimal_number);
  }
}

//-----------------------------------------------------------------------------
///
/// Prints the hexadecimal number.
///
/// @param size_of_hex_array
/// @param hexadecimal[CHAR_MAX]
//
void hexadecimalOut(int size_of_hex_array, int hexadecimal[CHAR_MAX])
{
  int count_hex;
  int count_zero;

  for(count_zero = 0; ;)
  {
    if(hexadecimal[count_zero] == 0)
    {
      count_zero++;
    }
    else if(hexadecimal[count_zero] != 0)
    {
      break;
    }
  }

  printf("0x");
  // Number is saved to an array.
  for(count_hex = count_zero; count_hex < size_of_hex_array; count_hex++)
  {
    printf("%x", hexadecimal[count_hex]);
  }
  printf("\n"); 
}


