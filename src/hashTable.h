#ifndef TAX_CALC_HASHTABLE_H
#define TAX_CALC_HASHTABLE_H

#define STRING_SIZE (-1)

typedef enum bucketState{occupied, empty, deleted} bucketState;

typedef struct bucket
{
	void *key;
	void *data;
	bucketState state;
} bucket;

typedef struct hashTable
{
	int keySize;
	int valueSize;
	int allocated;
	int indexesUsed;
	bucket *table;
} hashTable;

/*hash function for strings*/
static unsigned hashString(hashTable,void *);

/*hash function for fixed length data*/
static unsigned hash(hashTable, void *);

/*generates a new hashTable*/
hashTable newHashTable(int keySize, int valueSize);

/*return the value in the bucket with the key 'key'*/
void *readHash(hashTable, void *key);

/*write 'value' into the bucket with key 'key',
or create a new bucket with value 'value' and key 'key'
if bucket cannot be found*/
void writeHash(hashTable *, void *key, void *value);

/*erase the bucket with key 'key' if found*/
void eraseBucket(hashTable *, void *key);

/*returns 1 if a bucket with the key 'key' can
be found in the hashTable and 0 if it can't*/
int contains(hashTable, void *key);

/*frees the hashTable*/
void freeHashTable(hashTable);

#endif //TAX_CALC_HASHTABLE_H
