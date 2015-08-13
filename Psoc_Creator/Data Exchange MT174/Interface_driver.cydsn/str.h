/* ========================================
 *
 * Copyright FP UNA, 2015
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

/* [] END OF FILE */

#include <stdio.h>
#include <string.h>

// prototypes declarations
void split(char **parse, char *str, char *delimitators);
void substring(char* substr,char *string, int start, int end);

// definitions
void split(char **parse ,char *str, char* delimitators)
{
	size_t index = 0;
	char *token;
    
	token = strtok(str, delimitators);

	while(token != NULL)
	{
		*(parse + index++) = token;
		token = strtok(NULL, delimitators);
	}

	*(parse + index) = 0; 
}


void substring(char* substr, char *string, int start, int end) 
{  
	int large = strlen(string) -1; 
	int last; 
	char c[2]="";
    
	strcpy(substr,"");
	  
	if(end > 0 && end <= large) 
	{ 
	    last = end; 
	} 
	else if(end < 0) 
	{ 
	    last = large + end + 1; 
	} 
	else 
	{ 
	    last = large; 
	} 
	  
	for(start; start <= last; start++) 
	{ 
	    c[0] = string[start]; 
	    strcat(substr, c); 
	}
}
