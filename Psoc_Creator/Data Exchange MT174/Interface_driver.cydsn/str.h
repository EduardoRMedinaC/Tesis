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
#include <stdlib.h>

// prototypes declarations
char** split(char *str, char *delimitators);
char* substring(char *string, int start, int end);

// definitions
char** split(char *str, char* delimitators)
{
	int count = 0;
	size_t index = 0;
	char **parse;
	char *tmp = str;
	char *token;
	char *last_delimitator;

	while(*tmp)
	{
		/******************************************
		si *tmp contiene uno de los deliitadores
		strchr devolvera la direcccion de este sino
		devolvera una direccion nula
		*******************************************/
		if(strchr(delimitators, *tmp) != NULL)
		{
			count++;
			last_delimitator = tmp;

		}
		tmp++;
	}

	//Espacio para datos ubicados luego del ultimo delimitador
	count += last_delimitator < (str + strlen(str) - 1);

	//Espacio para el caracter nulo del arreglo de punteros
	count++;

	parse = malloc(sizeof(char*) * count);
	token = strtok(str, delimitators);

	while(token != NULL)
	{
		*(parse + index++) = token;
		token = strtok(NULL, delimitators);
	}

	*(parse + index) = 0;

	return parse; 
}


char* substring(char *string, int start, int end) 
{ 
	char *substr; 
	int large = strlen(string) -1; 
	int last; 
	char c[2]="";

	substr = (char *)malloc(sizeof(char) * (large+2));
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
	return substr; 
}
