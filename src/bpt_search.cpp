#include "bpt.h"
#include "bptnode.h"
#include "dbtypes.h"

#include <vector>
#include <cstring>
#include <iostream>

using namespace std;

void BPT::search(int lowerBoundKey, int upperBoundKey) //take in lower and upper range
{
  BPTNode *node = new BPTNode(getRoot()); // Traverse the tree, start with the root and move left and right accordingly
  bool stop = false; // default is false because not found any keys
  int totalavgrating = 0;
  int numavgrating = 0;

  if (node == nullptr) // tree contains no elements or tree is empty
  {
    return; // will not perform search since tree is empty
  }
  else // tree is not empty
  {
    while (!node->IsLeaf()) // while not reached a leaf node (not leaf node)
    {
      noofindexnodes++;

      for (int i = 0; i < node->GetNumKeys(); i++) // visit every key in the current node
      {

        if (lowerBoundKey < node->GetKey(i)) // if lowerBoundKey is less than current key (greater), go left.
        {
          node = new BPTNode(node->GetChild(i)); 
          break;
        }
        if (i == node->GetNumKeys() - 1) // if visited all the keys in current node (i.e., all keys are smaller), go right
        {
          node = new BPTNode(node->GetChild(i+1)); 
          break;
        }
      }
    }
    while (stop == false) // continue search over the entire range (reach leaf node)
    {
      int i;
      for (i = 0; i < node->GetNumKeys(); i++)
      {
  
        if (node->GetKey(i) > upperBoundKey) // continue till you reach upperBoundKey
        {
          stop = true;
          break; // reach the upper bound key
        }
        if (node->GetKey(i) >= lowerBoundKey && node->GetKey(i) <= upperBoundKey)
        {
          // print the movie record, found 
          char *pRecord = node->GetChild(i);
          int offset = (pRecord - storage_) % BLOCK_SIZE; 
          char *pBlockMem = pRecord - offset;
          RecordMovie *recordMovie = dbtypes::ReadRecordMovie(pBlockMem, offset);
          totalavgrating = recordMovie->avg_rating + totalavgrating;
          numofavgrating++;
          cout << "Movie Record -- tconst: " << recordMovie->tconst << " avgRating: " << recordMovie->avg_rating 
          << " numVotes: " << recordMovie->num_votes << endl;
        }
      }
    }
  }
  avgavgrating=totalavgrating/numofavgrating;
  return;
}