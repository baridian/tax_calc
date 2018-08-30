#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cstdarg>
#include <cmath>
#include <cctype>
#include <cstring>
#include "hashTable.h"

#define NUM_MONTHS 12

#define LONGEST_MONTH 10

#define COUNTY_TAX_RATE 0.0275

#define STATE_TAX_RATE 0.065

#define RIGHT_ALLIGN 6

char MONTH_NAMES[NUM_MONTHS][LONGEST_MONTH] = {"January","February","March","April","May",
								  "June","July","August","September","October",
								  "November","December"};

void initMonths(hashTable *map)
{
	int numMonths = 12;
	int irrelevantValue = 0;
	int i = 0;
	for(;i<numMonths;i++)
		writeHash(map,MONTH_NAMES[i],&irrelevantValue);
}

/*returns the minimum of n arguments, with n defined by count*/
int minimum(int count, ...)
{
	va_list argP;
	int min = INT_MAX;
	int i = 0;
	int currentValue;
	va_start(argP,count);
	for(;i<count;i++)
	{
		currentValue = va_arg(argP,int);
		if(currentValue < min)
			min = currentValue;
	}
	va_end(argP);
	return min;
}
	
/*calculates the levenshtein distance between two strings, that is:
the minimum number of weighted insertions, deletions, and substitutions to transform
one string into another. Weights have been set to work optimally with prefixes*/
int levenshtein(char *from, char *to)
{
	const int insertWeight = 1;
	const int subWeight = 4;
	const int deleteWeight = 4;
	int fromLength = (int)strlen(from) + 1;
	int toLength = (int)strlen(to) + 1;
	auto distanceTable = (int **)malloc(fromLength * sizeof(int *));
	int i = 0;
	int j;
	int subCost;
	int toReturn;
	for(;i<fromLength;i++) /*generate and zero table*/
		distanceTable[i] = (int *)calloc((size_t)toLength,sizeof(int));
	
	/*cost from empty string to non-empty is number of characters in 
	non-empty string*/
	for(i = 1;i < fromLength;i++)
		distanceTable[i][0] = i * deleteWeight;
	
	/*cost from empty string to non-empty is number of characters in 
	non-empty string*/
	for(j = 1; j < toLength;j++)
		distanceTable[0][j] = j * insertWeight;
	
	for(i = 1; i < fromLength;i++)
	{
		for(j = 1; j < toLength; j++)
		{
			if(from[i - 1] == to[j - 1]) /*if letters in string are equal*/
				subCost = 0;
			else
				subCost = subWeight;
			distanceTable[i][j] = minimum(3,distanceTable[i-1][j] + deleteWeight, /*deletion cost*/
                             			  distanceTable[i][j-1] + insertWeight, /*insertion cost*/
										  distanceTable[i-1][j-1]+subCost); /*substitution cost*/
		}
	}
	toReturn = distanceTable[fromLength - 1][toLength - 1];/*levenshtein cost in bottom corner*/
	for(i=0;i<fromLength;i++) /*free allocated memory*/
		free(distanceTable[i]);
	free(distanceTable);
	return toReturn;
}
			
			

int closestString(char stringArray[NUM_MONTHS][LONGEST_MONTH], int arrayLength, char *matching)
{
	int lowestDifference = INT_MAX;
	int lowestIndex = 0;
	int currentDifferenceCount = 0;
	int i = 0;
	for(;i < arrayLength;i++)
	{
		currentDifferenceCount = levenshtein(matching,stringArray[i]);
		if(currentDifferenceCount < lowestDifference)
		{
			lowestDifference = currentDifferenceCount;
			lowestIndex = i;
		}
	}
	return lowestIndex;
}
	
	

void getInput(hashTable validMonths, char *month, int *year, float *total)
{
	char tempReadTo[20];
	char closestMonth[20];
	int index;
	int invalid = 1;
	while(invalid)
	{
		printf("Enter the month: ");
		scanf("%20s",tempReadTo);
		while(getchar() != '\n'); /*clear stdin*/
		if(!contains(validMonths,tempReadTo)) /*if input is invalid*/
		{
			index = closestString(MONTH_NAMES,NUM_MONTHS,tempReadTo); /*find closest valid string*/
			strcpy(closestMonth,MONTH_NAMES[index]);
			printf("Error: '%s' is not a valid month. Did you mean '%s'?\n", /*print error*/
			       tempReadTo,closestMonth);
		}
		else
			invalid = 0;
	}
	strcpy(month,tempReadTo);
	invalid = 1;
	while(invalid)
	{
		printf("Enter the year: ");
		if(scanf("%d",year)) /*if value read in*/
			invalid = 0;
		else
		{
			while(getchar() != '\n'); /*clear stdin*/
			printf("Error: invalid year input.\n");
		}
	}
	
	invalid = 1;
	while(invalid)
	{
		printf("Enter the total: ");
		if(scanf("%f",total)) /*if value read in*/
		{
			if(abs((int)*total) < pow(10,RIGHT_ALLIGN - 1)) /*if total is too large to be displayed*/
				invalid = 0;
			else
				printf("Error: total too large.\n");
		}
		else
		{
			while(getchar() != '\n'); /*clear stdin*/
			printf("Error: invalid total.\n");
		}
	}
}

void calculate(float total, float *countyTax, float *stateTax, float *sales)
{
	if(total > 0)
	{
		*sales = total / (float)(1 + COUNTY_TAX_RATE + STATE_TAX_RATE);
		*countyTax = *sales * (float)COUNTY_TAX_RATE;
		*stateTax = *sales * (float)STATE_TAX_RATE;
	}
	else
	{ /*cannot have negative tax*/
		*sales = total;
		*countyTax = 0;
		*stateTax = 0;
	}
}

void print(char *month, int year, float total, float countyTax, float stateTax, float sales)
{
	printf("%s %d\n",month,year);
	printf("----------------\n");
	printf("Total Collected:  $%*.2f\n",RIGHT_ALLIGN + 2,total);
	printf("Sales:            $%*.2f\n",RIGHT_ALLIGN + 2,sales);
	printf("County Sales Tax: $%*.2f\n",RIGHT_ALLIGN + 2,countyTax);
	printf("State Sales Tax:  $%*.2f\n",RIGHT_ALLIGN + 2,stateTax);
	printf("Total Sales Tax:  $%*.2f\n",RIGHT_ALLIGN + 2,countyTax + stateTax);
}

int requestedExit(int needsClear)
{
	char read;
	printf("Do you wish to perform another calculation? (Y/N): ");
	if(needsClear)
		while(getchar() != '\n');
	scanf("%c",&read);
	while(getchar() != '\n');
	read = (char)toupper(read);
	switch(read)
	{
		case 'Y':
		return 0; /*exit not requested if they want to calculate again*/
		
		case 'N':
		return 1; /*exit requested if they do not*/
		
		default: /*if character is not Y or N repeat*/
		printf("Error: '%c' is not a valid character\n",read);
		return requestedExit(0);
	}
}
		

int main(int argc, char *argv[])
{
	/*hash table used for checking validity of month due to hash table
	 average set confirmation time of O(1), compared to linear search
	 O(n) or b-tree O(log(n)). Poor worst case performance of O(n) for
	 hash table is insignificant since program is not real-time*/
	hashTable validMonths = newHashTable(STRING_SIZE,sizeof(int));
	char month[20];
	int year;
	float total;
	float countyTax;
	float stateTax;
	float sales;
	initMonths(&validMonths);
	while(1)
	{
		getInput(validMonths,month,&year,&total);
		calculate(total,&countyTax,&stateTax,&sales);
		print(month,year,total,countyTax,stateTax,sales);
		if(requestedExit(1))
			break;
	}
	freeHashTable(validMonths);
	return 0;
}