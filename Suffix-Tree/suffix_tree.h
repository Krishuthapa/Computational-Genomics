#ifndef SUFFIXTREE_H
#define SUFFIXTREE_H

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <sstream>

using namespace std;


struct Node{
    int nodeDepth;
    int strDepth;
    int id;

    bool isLeaf;
    bool isInternalNodes;
    
    Node *parent;
    Node *suffix_link;

    vector<Node*> children;    
    vector<int> edgeIndices;
    vector<int> pathlabel;
};

struct Tree{
   Node* root;
};


class SuffixTree{
   private:
      string inputString;
      Tree *tree;
      string substring;

      Node *deepestNode;

      ofstream out_file;
      ofstream bwt_output;

      int leafId;
      int nodeId;

      vector<int> internalNodesStringDepth;

      int internalNodeCount = 0;
      int leavesCount = 0;

      bool is_root(Node* input_node);

      /* Function implemented for displaying the output text with the char limit for a line. */
      void displayTextInOutputWithCharLimitInLine(int startIndex, int strCount , int lineCharLength);

      /* Gives us the matched child for the given node and the substring. */
      Node *getMatchedChild(Node *currentNode );

      /* Gives us the matched child index for the given node and the substring. */
      int getMatchedChildIndex(Node *parent , Node *matchedChild );

      /* Gives the index on where to insert the new child in the node. Maintains the lexicographical sorting order. */
      int getInsertIndexForNewChild(Node *currentNode);

      /* Updates node  for all the children when a new node is added. */
      void updateDepth(Node* node, int depth);

      /* Function implementing the DFS.*/
      void DFS(Node *node, int &count);

      /* Function implementing the DFS for BWT index.*/
      void DFSBWT(Node *node);


      /* Adds new child to a node if no path matches the first character of the new substring. */
      void addNewChildNode(Node *u, int start_index , int end_index);


      /* Function to hop to the next internal node if no mismatch is found and edge is exhausted for the current node. */
      Node* traverseFurther(Node* currentNode , int currentSubstringIndex , int currentNodeStartIndex , int currentNodeEndIndex);


      /* When a mismatch is found along the path a new internal node is added to the specific mismatch location. */
      Node *addInternalNode(Node* currentMatchedNode , int currentMatchedNodeIndex, int breakDistance, 
                           int currentMatchNodeStartIndex);
      
      /* Implements the NodeHops for the case II of suffix link implementation. */
      Node* NodeHops(Node *input_node, int startIndex, int endIndex);

      // Traverses and finds the path along which new nodes are to be added and does the addition.
      Node *findPath(Node *u, int start_index , int end_index);

      /* Inserts the new substring to the old suffix tree and returns the updated old tree with new substring. */
      Tree *insert(string input, Tree *oldTree);

   public:
      SuffixTree(string input_string, string filename);

      // Builds the tree.
      void buildTree();

      /* Implements the each node and its children in its depth first manner nad enumerate it with string depth*/
      void implementDFSAndEnumerate();
      
      /* Gives the average string depths for the leaves. */
      int getInternalNodeAverageStringDepth();

      /* Provides the statistics for the tree. */
      void getStatistics();

      /* Display the children information of a given internal node. */
      void displayChildren(Node *currentNode);

      /* Gets the BWT Index for the Suffix Tree. */
      void getBWTIndex();

};


#endif // SUFFIXTREE_H