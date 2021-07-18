#include <iostream>
#include <cmath>
#include <string>
#include <fstream>

using namespace std;

// cache Block 
class Block
{
    public:
        bool* tag;       // tag field
        bool valid;      // block is filled or not
        bool dirty;      // any write operations has been performed or not
        Block* next;     // next block
    
    public:
        Block(int);
        Block();
};

Block::Block()
{
    valid = 0;
    dirty = 0;
    next = NULL;
}
// for creating cache && request block
Block::Block(int tagSize)
{
    tag = new bool[tagSize];
    valid = 0;
    dirty = 0;
    next = NULL;
}

// Block List (to store set of cache)
class BlockList
{
    public:
        Block* head;        // first element
        int size;           // size;
        BlockList()         
        {
            head = NULL;   
        }
};

class Cache: public Block
{
    private:
        // cache
        BlockList* cache;                          
        // replacement functions
        void Random(Block, int, bool);            
        void LeastRecentlyUsed(Block, int, bool);
        void PseudoLRU(Block, int, bool);
        // for pseusoLRU
        int** setTree;
    
    public:
        // cache configuration
        int totalBlocks;
        int filledBlocks;
        int totalSize;
        int blockSize;
        int associativity;
        int replacementPolicy;
        int tagSize;
        int sets;

        // cache statistics
        int cacheAccesses;
        int readAccesses;
        int writeAcceses;
        int cacheMisses;
        int compulsoryMisses;
        int capacityMisses;
        int conflictMisses;
        int readMisses;
        int writeMisses;
        int dirtyBlocksEvicted;

        // create cache with given configuration
        Cache(int, int, int, int);
        ~Cache();
        // to find requested block in cache
        void checkInCache(Block, int, bool);
};

Cache::Cache(int tSize, int bSize, int way, int rPolicy)
{
    // initialise all information of cache 
    totalSize = tSize;
    blockSize = bSize;
    associativity = way;
    replacementPolicy = rPolicy;
    // calculate number of sets for given cache
    if(associativity != 0)
    {
        sets = (totalSize/(blockSize*associativity));
    }
    else
    {
        sets = 1;
    }
    tagSize = 32-log2(blockSize)-log2(sets);

    // initialise all stats of cache 
    cacheAccesses = 0;
    cacheMisses = 0;
    readAccesses = 0;
    readMisses = 0;
    writeAcceses = 0;
    writeMisses = 0;
    compulsoryMisses = 0;
    capacityMisses = 0;
    conflictMisses = 0;
    dirtyBlocksEvicted = 0;
    totalBlocks = totalSize/blockSize;
    filledBlocks = 0;

    // allocate memory
    cache = new BlockList[sets];
    // for direct mapped and set associative
    if(associativity != 0)
    {
        for(int i=0;i<sets;i++)
        {
            for(int j=0;j<associativity;j++)
            {
                Block* temp = cache[i].head;
                cache[i].head = new Block(tagSize);
                cache[i].head->next = temp;
            }
        }
    }
    else // for full associative
    {
        for(int i=0;i<totalBlocks;i++)
        {
            Block* temp;
            temp = new Block(tagSize);
            temp->next = cache[0].head;
            cache[0].head = temp;
        }
    }
    // for pseudo lru
    if(replacementPolicy == 2 && associativity >= 2)
    {
        setTree = new int*[sets];
        for(int i=0;i<sets;i++)
        {
            setTree[i] = new int[associativity-1];
            for(int j=0;j<associativity-1;j++)
            {
                setTree[i][j] = 0;
            }
        }
    }
}
Cache::~Cache()
{   
    // deallocate memory allocated to 
    // tag of each block
    // then deallocate memory of cache
    if(associativity != 0) // for direct mapped & set-associative
    {
        for(int i=0;i<sets;i++)
        {
            Block* temp = cache[i].head;
            while(temp != NULL)
            {
                delete[] temp->tag;
                temp = temp->next;
            }
        }
    }
    else    // fully associative
    {
        Block* temp = cache[0].head;
        while(temp!=NULL)
        {
            delete[] temp->tag;
            temp = temp->next;
        }
    }
    delete[] cache;

    // deallocation of memory allocated to 
    // tree of each set
    if(associativity >= 2 && replacementPolicy == 2)
    {
        for(int i=0;i<sets;i++)
        {
            delete[] setTree[i];
        }
        delete[] setTree;
    }
}

void Cache::Random(Block request, int setIndex, bool writeRequest)
{
    // check if request is present or not 
    // dirty bit 

    // if not perform random replacment 
    // increment counters of misses appropriately 
    // deallocate memory in case of deletion of block
    // using tag->next = tag->next->next
    // dirty bit
    // filled blockes etc... 
}

void Cache::LeastRecentlyUsed(Block request, int setIndex, bool writeRequest)
{
    // check if request is present or not 
    // dirty bit 

    // if not lru replacment 
    // increment counters of misses appropriately 
    // deallocate memory in case of deletion of block 
    // using tag->next = tag->next->next
    // dirty bit
    // filled blockes etc...
}

void Cache::PseudoLRU(Block request, int setIndex, bool writeRequest)
{
    if(associativity>=2)
    {
        bool found = true;
        Block* temp = cache[setIndex].head;
        int index = associativity-1;
        // loop to check if request is present or not 
        while(temp != NULL)
        {
            bool matched = true;
            if(temp->valid)
            {
                for(int i=0;i<tagSize;i++)
                {
                    if(temp->tag[i] != request.tag[i])
                    {
                        matched = false;
                        break;
                    }
                }
            }
            else matched = false;
            if(matched)
            {
                found = true;
                break;
            }
            else
            {
                found = false;
            }
            index++;
            temp = temp->next;
        }

        // if found 
        if(found)
        {
            if(writeAcceses)
            {
                temp->dirty = 1; // set dirty bit
            }
            // invert all bits till root node
            while(index > 0)
            {
                if(ceil((float)(index-1)/2)-(index-1)/2 == 1)
                {
                    setTree[setIndex][(index-1)/2] = 0;
                }
                else setTree[setIndex][(index-1)/2] = 1;
                index = (index-1)/2;
            }
        }
        else
        {
            cacheMisses++;
            index = 0;
            // find index of block to be replced 
            while(index < associativity-1)
            {
                index = 2*index+1+setTree[setIndex][index];
            }
            Block* temp = cache[setIndex].head;
            index -= (associativity-1);
            // loop to find block address 
            for(int i=0;i<index;i++)
            {
                temp = temp->next;
            }

            // loop to invert all the bits till parent node
            index += (associativity-1);
            while(index>0)
            {
                index = (index-1)/2;
                if(setTree[setIndex][index]==1)
                {
                    setTree[setIndex][index]==0;
                }
                else setTree[setIndex][index]=1;
            }

            // miss count
            if(totalBlocks>filledBlocks)
            {
                if(temp->valid)
                {
                    conflictMisses++;
                }
                else compulsoryMisses++;
            }
            else capacityMisses++;  

            if(temp->dirty == 1)
            {
                dirtyBlocksEvicted++;
            }
            
            temp->valid = 1;
            for(int i=0;i<tagSize;i++)
            {
                temp->tag[i] = request.tag[i];
            }
            
            if(writeAcceses)
            {
                temp->dirty = 1 ;
                writeMisses++;
            }
            else
            {
                temp->dirty = 0;
                readMisses++;
            }
        }
    }
}

void Cache::checkInCache(Block request, int setIndex, bool writeRequest)
{
    bool found = true; // request is found or not

    // increment appropriate counters 
    cacheAccesses++;
    if(writeRequest) writeAcceses++;
    else readAccesses++;

    // if cache is direct mapped
    if(associativity == 1)
    {
        // temp : to store head pointer of set
        Block* temp = cache[setIndex].head;
        // if block is not empty 
        if(temp->valid == 1)
        {
            // check the tag is same or not
            for(int i=0;i<tagSize;i++)
            {
                if(temp->tag[i] != request.tag[i])
                {
                    found = false;
                    break;
                }
            }
        }
        else // if empty then a new block will be fetched
        {
            found = false;  // not found
            filledBlocks++; // increment filled blocks
        }

        // if found and request is a write request
        if(found && writeRequest)
        {
            temp->dirty = 1; // set the dirty bit
        }
        // if not found 
        else if(!found)
        {
            // increment cache miss
            cacheMisses++;

            // if the block is dirty then 
            // increment dirtyBlocksEvicte counter
            if(temp->dirty==1)
            {
                dirtyBlocksEvicted++;
            }

            // copy tag of request to the block
            for(int i=0;i<tagSize;i++)
            {
                temp->tag[i] = request.tag[i];       
            }
            // set valid bit
            temp->valid = 1;

            // if it is a write request then
            // set dirty bit = 1
            // increment appropriate counters 
            if(writeRequest)
            {
                temp->dirty = 1;
                writeMisses++;
            }
            else
            {
                temp->dirty = 0;
                readMisses++;
            }
            //**************
            // BUGGGGGGG !!!!!!
            //**************
            // increment capacity/ conflict/ compulsory Misses appropriately
            // not sure 
            if(totalBlocks > filledBlocks)
            {
                if(temp->valid)
                {
                    conflictMisses++;
                }
                else compulsoryMisses++;
            }
            else capacityMisses++;
        }
    }
    else // for fully associative and set associative cache 
    {
        // call appropritate function
        if(replacementPolicy == 0)
        {
            Random(request, setIndex, writeRequest);
        }
        else if(replacementPolicy == 1)
        {
            LeastRecentlyUsed(request, setIndex, writeRequest);
        }
        else
        {
            PseudoLRU(request, setIndex, writeRequest);
        }
    }
}

int main()
{
    // variables for cache 
    ifstream cacheConfigure;  // input stream
    string  traceFileName;    // trace file name
    int cacheSize;            // cache size
    int bSize;                // block size
    int associativity;        // associativity
    int rPolicy;              // replacement policy

    // open input file 
    cacheConfigure.open("input.txt");
    
    // scan congifuration of cache
    cacheConfigure >> cacheSize >> bSize;
    cacheConfigure >> associativity >> rPolicy;
    cacheConfigure >> traceFileName;

    // create cache as described in input file
    Cache cache(cacheSize, bSize, associativity, rPolicy);

    // close input file
    cacheConfigure.close();

    ifstream traces;                // trace file stream
    traces.open(traceFileName);     // open file
    long address;                   // address of request 
    long setBits = log2(cache.sets);// number of bits to indicate set Index
    long offset = log2(bSize);      // offset bits
    Block request(cache.tagSize);   // request block with given tag size

    // scan in hexadecimal format
    while(traces >> hex >>address)
    {
        char readOrWrite;       // to store request type
        bool writeRequest;      // flag of request
        traces >> readOrWrite;  // scan the request type
        // decide flag
        if(readOrWrite == 'r') writeRequest = false;
        else writeRequest = true;

        // calculate set Index  
        int setIndex = 0;
        address = address >> offset;    // remove offset number of bis from address

        // loop to calculate setIndex
        for(int i=0;i<setBits;i++)
        {
            setIndex += (address&1)*pow(2,i);
            address = address >> 1;
        }

        // loop to calculate tag value
        for(int i=0;i<cache.tagSize;i++)
        {
            request.tag[cache.tagSize-i-1] = (address&1);
            address = address >> 1;
        }

        // procees the request
        cache.checkInCache(request, setIndex, writeRequest);
    }
    // delete memory allocated to request tag 
    delete[] request.tag;
    // close trace file
    traces.close();

    ofstream output;            // stream to output file
    output.open("output.txt");

    // display configuration as described
    output << cache.totalSize << endl;
    output << cache.blockSize << endl;
    if(cache.associativity == 0)
    {
        output << "Fully-associative cache" << endl;
    }
    else if(cache.associativity == 1)
    {
        output << "Direct-mapped cache" << endl;
    }
    else output << "Set-associative cache" << endl;

    if(cache.replacementPolicy == 0)
    {
        output << "Random Replacement" << endl;
    }
    else if(cache.replacementPolicy == 1)
    {
        output << "LRU Replacement" << endl;
    }
    else output << "Pseudo Replacement" << endl;
    // display statistics as described
    output << cache.cacheAccesses << endl;
    output << cache.readAccesses  << endl;
    output << cache.writeAcceses  << endl;
    output << cache.cacheMisses   << endl;
    output << cache.compulsoryMisses << endl;
    output << cache.capacityMisses << endl;
    output << cache.conflictMisses << endl;
    output << cache.readMisses    << endl;
    output << cache.writeMisses   << endl;
    output << cache.dirtyBlocksEvicted << endl;

    // close the file
    output.close();

    return 0;
}



