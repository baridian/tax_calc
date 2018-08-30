#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include "hashTable.h"

static const int START_SIZE = 4;

static const float MAX_LOAD = 0.7;

static unsigned hashString(hashTable table, void *key)
{
	char *keyCopy = (char *)key;
	unsigned toReturn = 0;
	int i = 0;
	for(; keyCopy[i];i++)
		toReturn += keyCopy[i] * pow(31,i);
	return toReturn % table.allocated;
}

static unsigned hash(hashTable table, void *key)
{
	unsigned toReturn = 0;
	int i = 0;
	for (; i < table.keySize; i++)
		toReturn += ((char *) key)[i] * pow(31, table.keySize - i - 1);
	return toReturn % table.allocated;
}

hashTable newHashTable(int keySize, int valueSize)
{
	hashTable toReturn;
	int i = 0;
	toReturn.allocated = START_SIZE;
	toReturn.indexesUsed = 0;
	toReturn.keySize = keySize;
	toReturn.valueSize = valueSize;
	toReturn.table = (bucket *) malloc(sizeof(bucket) * START_SIZE);
	for (; i < START_SIZE; i++) /*all buckets are empty at generation*/
		toReturn.table[i].state = empty;
	return toReturn;
}

void *readHash(hashTable table, void *key)
{
	int index = table.keySize == STRING_SIZE ? hashString(table,key) : hash(table,key);
	int start = index;
	do
	{
		if(table.table[index].state == occupied)
		{
			/*if keys are not strings and keys match*/
			if (table.keySize != STRING_SIZE && memcmp(key, table.table[index].key, table.keySize) == 0)
				return table.table[index].data;
			/*elise if keys are strings and keys match*/
			else if (table.keySize == STRING_SIZE && strcmp((char *) table.table[index].key, (char *) key) == 0)
				return table.table[index].data;
		}
		index = ++index % table.allocated;
		/*while have not hit an empty index or visited every node*/
	} while (table.table[index].state != empty && index != start);
	return NULL;
}

static void rescaleHash(hashTable *table)
{
	bucket *oldTable;
	int oldLength;
	int i = 0;
	int newIndex;
	/*if number of indexes exceeds maximum tolerated load or is below the max load if the table size is halved,*/
	if (table->indexesUsed > table->allocated * MAX_LOAD || table->indexesUsed < table->allocated * MAX_LOAD / 2)
	{
		oldTable = table->table;
		oldLength = table->allocated;
		/*if exceeding maximum load then double amount allocated,
		else halve amount allocated*/
		if(table->indexesUsed > table->allocated * MAX_LOAD)
			table->allocated *= 2;
		else if(table->allocated > START_SIZE)
			table->allocated /= 2;
		/*reassign table pointer to memory space of new amount*/
		table->table = (bucket *) malloc(sizeof(bucket) * table->allocated);
		/*every bucket is empty*/
		for (; i < table->allocated; i++)
			table->table[i].state = empty;
		/*iterate through old table*/
		for (i = 0; i < oldLength; i++)
		{
			/*if current bucket is occupied*/
			if (oldTable[i].state == occupied)
			{
				/*rehash and place in first empty bucket*/
				newIndex = table->keySize == STRING_SIZE ? hashString(*table, oldTable[i].key) : hash(*table,oldTable[i].key);
				while (table->table[newIndex].state == occupied)
					newIndex = ++newIndex % table->allocated;
				table->table[newIndex] = oldTable[i];
			}
		}
		free(oldTable);
	}
}

void writeHash(hashTable *table, void *key, void *value)
{
	int index = table->keySize == STRING_SIZE ? hashString(*table,key) : hash(*table,key);
	int start = index;
	/*while have not hit an empty bucket*/
	while (table->table[index].state != empty)
	{
		/*if current bucket's key matches break*/
		if(table->table[index].state == occupied)
		{
			if(table->keySize != STRING_SIZE && memcmp(table->table[index].key,key,table->keySize) == 0)
				break;
			else if(table->keySize == STRING_SIZE && strcmp((char *)table->table[index].key,(char *)key) == 0)
				break;
		}
		
		index = ++index % table->allocated;
		if(index == start)
		{
			fprintf(stderr,"ERROR: table is full. This should never happen\n");
			exit(1);
		}
	}
	/*write or overwrite to bucket*/
	table->table[index].state = occupied;
	if(table->keySize != STRING_SIZE)
	{
		table->table[index].key = malloc((unsigned) table->keySize);
		memcpy(table->table[index].key, key, (unsigned) table->keySize);
	}
	else
	{
		table->table[index].key = malloc(strlen((char *)key) + 1);
		strcpy((char *)table->table[index].key,(char *)key);
	}
	table->table[index].data = malloc((unsigned) table->valueSize);
	memcpy(table->table[index].data, value, (unsigned) table->valueSize);
	table->indexesUsed++;
	rescaleHash(table); /*number of indexes changed; attempt rescale*/
}

void eraseBucket(hashTable *table, void *key)
{
	int index = table->keySize != STRING_SIZE ? hash(*table,key) : hashString(*table,key);
	int start = index;
	int found = 0;
	/*repeat until empty bucket is hit*/
	while (table->table[index].state != empty)
	{
		/*if bucket is occupied see if keys match. set found to 1 and break if they do*/
		if(table->table[index].state == occupied)
		{
			if(table->keySize != STRING_SIZE && memcmp(table->table[index].key,key,table->keySize) == 0)
			{
				found = 1;
				break;
			}
			else if(table->keySize == STRING_SIZE && strcmp((char *)table->table[index].key,(char *)key) == 0)
			{
				found = 1;
				break;
			}
		}
		/*else move to next bucket, and if every bucket has been visited key cannot be found so return*/
		index = ++index % table->allocated;
		if(index == start)
			return;
	}
	/*if the key was matched then erase it*/
	if(found)
	{
		table->table[index].state = deleted;
		free(table->table[index].key);
		free(table->table[index].data);
		table->indexesUsed--;
		rescaleHash(table);
	}
}

int contains(hashTable table, void *key)
{
	if(readHash(table,key) != NULL)
		return 1;
	return 0;
}

void freeHashTable(hashTable table)
{
	int i = 0;
	for (; i < table.allocated; i++)
	{
		if (table.table[i].state)
		{
			free(table.table[i].key);
			free(table.table[i].data);
		}
	}
	free(table.table);
}