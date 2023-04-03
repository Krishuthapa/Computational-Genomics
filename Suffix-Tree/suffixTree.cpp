#include "suffix_tree.h"

   bool SuffixTree::is_root(Node* input_node) {
      if (input_node == this->tree->root) {
            return true;
         }
      return false;
   }

   /* Function implemented for displaying the output text with the char limit for a line. */
   void SuffixTree::displayTextInOutputWithCharLimitInLine(int startIndex, int strCount , int lineCharLength){

         string operatedString = this->inputString.substr(startIndex, strCount);
         int count = 0;

         for(char base : operatedString){
            if(count % lineCharLength == 0){
               this->out_file<< "\n" << endl;
            }

            this->out_file << base;
            
            count++;
         }
      }

      /* Gives us the matched child for the given node and the substring. */
      Node *SuffixTree::getMatchedChild(Node *currentNode ){
         Node *matchedChild = nullptr;

         /* This portion of the code gives the matching child for the given node children and substring. */
         for (Node *child : currentNode->children){
            if (!child->pathlabel.empty() && this->inputString[child->edgeIndices[0]] == this->substring[0]){
               matchedChild = child;
            }
         }

         return matchedChild;
      }

      /* Gives us the matched child index for the given node and the substring. */
      int SuffixTree::getMatchedChildIndex(Node *parent , Node *matchedChild ){
         int match_index;

         int count = 0;

         for (Node *child : parent->children){
            if(child == matchedChild){
               match_index  = count;
               break;
            }

            count++;
         }

         return match_index;
      }

      /* Gives the index on where to insert the new child in the node. Maintains the lexicographical sorting order. */
      int SuffixTree::getInsertIndexForNewChild(Node *currentNode){
         vector<char> childStartChar;

         int insert_index;

         for (Node *child : currentNode->children){
            char comparisionChar = this->inputString[child->edgeIndices[0]];

            if(this->substring[0] != comparisionChar){
               childStartChar.push_back(comparisionChar);
            }
         }

         childStartChar.push_back(this->substring[0]);

         sort(childStartChar.begin(),childStartChar.end());

         for(int i = 0 ; i < childStartChar.size(); i++){
            if(childStartChar[i] == this->substring[0]){
               insert_index = i;
            }
         }

         return insert_index;
      }

      /* Updates node  for all the children when a new node is added. */
      void SuffixTree::updateDepth(Node* node, int depth) {
         node->nodeDepth = depth;          
         for (Node* child : node->children) {
               updateDepth(child, depth+1); 
         }
      }


      /* Function implementing the DFS.*/
      void SuffixTree::DFS(Node *node, int &count){
         if (node == nullptr) {
            return;
         }
         
         if(node->isLeaf){
            if(count % 10 == 0){
               this->out_file << "\n";
            }

            this->out_file << node->strDepth << "\t";
            count++;
         }
         
         for (Node* child : node->children) {
            DFS(child,count); 
         }
      }

      /* Function implementing the DFS for BWT index.*/
      void SuffixTree::DFSBWT(Node *node){
         if (node == nullptr) {
            return;
         }

         int index;

         if(node->id == 0){
            index = this->inputString.length() - 1;
         }else{
            index = (node->id - 1);
         }

         if(node->isLeaf){
            this->bwt_output << this->inputString[index] << endl;

            return;
         }

         for (Node* child : node->children) {
            DFSBWT(child); 
         }
      }


      /* Adds new child to a node if no path matches the first character of the new substring. */
      void SuffixTree::addNewChildNode(Node *u, int start_index , int end_index){
         int insert_index =  getInsertIndexForNewChild(u);

         Node* newNode = new Node;

         newNode->parent = u;

         newNode->nodeDepth = newNode->parent->nodeDepth +1;
         newNode->isLeaf = true;

         newNode->id = this->leafId;
         this->leafId++;
         this->leavesCount++;
            
         newNode->edgeIndices.push_back(start_index);
         newNode->edgeIndices.push_back(end_index);


         int pathlabelStartIndex = start_index - u->strDepth;
         int pathlabelEndIndex = end_index;
         
         newNode->pathlabel.push_back(pathlabelStartIndex);
         newNode->pathlabel.push_back(pathlabelEndIndex);

         newNode->strDepth = pathlabelEndIndex - pathlabelStartIndex + 1;
         newNode->suffix_link = nullptr;
            
         u->children.insert(u->children.begin() + insert_index, newNode);
         
         return;
      }


      /* Function to hop to the next internal node if no mismatch is found and edge is exhausted for the current node. */
      Node* SuffixTree::traverseFurther(Node* currentNode , int currentSubstringIndex , int currentNodeStartIndex , int currentNodeEndIndex){
         
         int clippedStartIndex = currentNodeStartIndex + currentSubstringIndex;
         
         Node* node = findPath(currentNode, clippedStartIndex , currentNodeEndIndex);

         return node;
      }


      /* When a mismatch is found along the path a new internal node is added to the specific mismatch location. */
      Node *SuffixTree::addInternalNode(Node* currentMatchedNode , int currentMatchedNodeIndex, int breakDistance, 
                           int currentMatchNodeStartIndex){
         
         Node *newInternalNode = new Node;

         // Storing the path label as the vector of start and end indices.
         int pathLabelStartIndex = currentMatchedNode->edgeIndices[0] - currentMatchedNode->parent->strDepth;
         int pathLabelEndIndex = currentMatchedNode->edgeIndices[0] + breakDistance - 1 ; 

         int newNodeStartIndex = currentMatchNodeStartIndex + breakDistance;

         currentMatchedNode->parent->children[currentMatchedNodeIndex] = newInternalNode;

         newInternalNode->edgeIndices.push_back(currentMatchedNode->edgeIndices[0]);
         newInternalNode->edgeIndices.push_back(currentMatchedNode->edgeIndices[0] + breakDistance - 1);

         newInternalNode->pathlabel.push_back(pathLabelStartIndex);
         newInternalNode->pathlabel.push_back(pathLabelEndIndex);

         newInternalNode->parent = currentMatchedNode->parent;
         newInternalNode->nodeDepth = currentMatchedNode->nodeDepth;
         newInternalNode->isLeaf = false;
         newInternalNode->isInternalNodes = true;
         newInternalNode->strDepth = pathLabelEndIndex - pathLabelStartIndex + 1;

         newInternalNode->suffix_link = nullptr;

         newInternalNode->id = this->nodeId;
         this->nodeId++;
         this->internalNodeCount++;
         
         if(this->deepestNode == nullptr ||  (this->deepestNode != nullptr && 
            newInternalNode->strDepth > this->deepestNode->strDepth)){

            this->deepestNode = newInternalNode;
         }

         this->internalNodesStringDepth.push_back(newInternalNode->strDepth);
         
         /* Updating the current matched node. */
         currentMatchedNode->parent = newInternalNode;
         currentMatchedNode->edgeIndices[0] = newInternalNode->edgeIndices[1] + 1;

         updateDepth(currentMatchedNode,newInternalNode->nodeDepth + 1);  
         newInternalNode->children.push_back(currentMatchedNode);

         return newInternalNode;
      }

      
      /* Implements the NodeHops for the case II of suffix link implementation. */
      Node* SuffixTree::NodeHops(Node *input_node, int startIndex, int endIndex) {
       
         string beta = this->inputString.substr(startIndex, endIndex-startIndex+1);
            if (beta.length() == 0) {
               return input_node;
            }

            Node *nextHop = nullptr;
            for (Node *child : input_node->children) {
               char firstChildChar = this->inputString[child->edgeIndices[0]];
               if (firstChildChar == beta[0]) {
                  nextHop = child;
                  break;
               }
            }
            
            //beta aligns with the next hop
            int hoplength = nextHop->edgeIndices[1] + 1 - nextHop->edgeIndices[0];
            if (beta.length() == hoplength) {
               return nextHop;
            }

            // beta is longer than the next hop
            else if (beta.length() > hoplength) {
               return NodeHops(nextHop, startIndex + hoplength, endIndex);
            }

            // beta is smaller than the stringdepth, make a new internal node
            else {
               int matchedChildIndex = getMatchedChildIndex(input_node , nextHop);
               int start_index = nextHop->edgeIndices[0] + beta.length();
               Node * newInternalNode = addInternalNode(nextHop, matchedChildIndex , beta.length(), start_index);
               return newInternalNode;
            }   
      }     


      // Traverses and finds the path along which new nodes are to be added and does the addition.
      Node *SuffixTree::findPath(Node *u, int start_index , int end_index){
         
         this->substring = this->inputString.substr(start_index, end_index - start_index + 1);
        
         Node *matchedChild = getMatchedChild(u);
         Node *newnode;
         int matchedChildIndex = getMatchedChildIndex(u,matchedChild);
         

         /* This portion of code is to add the leaves to a internal node u if it has no path with the first letter of current substring.*/
         if(matchedChild == nullptr){
            addNewChildNode(u, start_index , end_index); 
            return u;
         }

         /* The following block implements for the matched child node for the given substring. */  
         int breakTermDistance = -1;
         int substrCount = 0;

         for(int index = matchedChild->edgeIndices[0] ; index <= matchedChild->edgeIndices[1]; index++ ){
               
            if(this->substring[substrCount] != this->inputString[index]){
               breakTermDistance = substrCount;
               break;
            }

            substrCount++;

         }

         int edgeLabelLength = matchedChild->edgeIndices[1] - matchedChild->edgeIndices[0] + 1;

         /* Traverse to another child node for the possible path if the edge is exhausted but the substring is not with no mismatch. */
         if(breakTermDistance == -1  && this->substring.length() > edgeLabelLength){
            newnode = traverseFurther(matchedChild,substrCount, start_index, end_index);

            return newnode;
         }

         /*Add a new node to  between the matched child and its parent at the mistmatch break point.*/
         if(breakTermDistance != -1){
            newnode = addInternalNode(matchedChild, matchedChildIndex , breakTermDistance, start_index);
            /* For the remaining part of substring after the new node is added at the mismatch we again run the find path function along the newly added node.*/    
            findPath(newnode , start_index+breakTermDistance , end_index);
            return newnode;
         }

         return u;
      }

      /* Inserts the new substring to the old suffix tree and returns the updated old tree with new substring. */
      Tree *SuffixTree::insert(string input, Tree *oldTree){
         int end_index = this->inputString.length() -1;
         int count = 1;

         Node* u = nullptr;
         u = findPath(oldTree->root , 0 , end_index);
          
         Node* uprime = oldTree->root;
         
         while (count < this->inputString.length()) {
            int start_index = count;
            
            //sufffix link is known
            if (u->suffix_link != nullptr) { 
               //if parent is root (CASE - 1B)
               if (is_root(u)) {
                  u = findPath(oldTree->root, start_index , end_index);
               }

               //if parent is not root (CASE - 1A)
               
               else {
                  Node *v = u->suffix_link;
                  start_index = start_index + v->strDepth;
                  u = findPath(v, start_index , end_index);
               }
            }

            //sufffix link is unknown
            else {
               Node *uprime = u->parent;
               Node *vprime = uprime->suffix_link;
               Node *v;
               
               //if parent is root (CASE - 2B)
               if (is_root(uprime)) {
                  v = NodeHops(vprime,u->edgeIndices[0]+1,u->edgeIndices[1]);
               }

               //if parent is not root (CASE - 2A)
               else {
                  v = NodeHops(vprime,u->edgeIndices[0],u->edgeIndices[1]);   
               }
               
               u->suffix_link = v;
               start_index = start_index + v->strDepth;
               u = findPath(v, start_index , end_index);
            }
            
            count++;            
         }  

         return oldTree;
      }

      SuffixTree::SuffixTree(string input_string, string filename){
         this->inputString = input_string;

         Node *root = new Node;

         root->isLeaf = false;
         root->parent = nullptr;
         root->suffix_link = root;
         root->strDepth = 0;
         root->nodeDepth = 0;
         root->id = input_string.length();


         this->out_file.open(filename + ".out",ios::app);
         this->bwt_output.open(filename + ".BWToutput.txt",ios::app);
         
         this->tree = new Tree;
         this->tree->root = root;

         this->nodeId = input_string.length() + 1;
         this->leafId = 0;
         this->deepestNode = this->tree->root;
      }

      // Builds the tree.
      void SuffixTree::buildTree(){
         this->tree = insert(this->inputString, this->tree);        
      }

      /* Implements the each node and its children in its depth first manner nad enumerate it with string depth*/
      void SuffixTree::implementDFSAndEnumerate(){
         int count = 0;
         this->out_file << "Enumerated String Depths:" << endl;
         DFS(this->tree->root, count);
      }
      
      /* Gives the average string depths for the leaves. */
      int SuffixTree::getInternalNodeAverageStringDepth(){
         int sum = accumulate(this->internalNodesStringDepth.begin(), this->internalNodesStringDepth.end(),0.0);

         double average = (double)sum / this->internalNodesStringDepth.size();

         return (int)average;
      }

      /* Provides the statistics for the tree. */
      void SuffixTree::getStatistics(){

         int averageStringDepths = getInternalNodeAverageStringDepth();

         this->out_file << "===========================================\n";
         this->out_file << "Statistics of the Built Tree:" <<endl;

         this->out_file << "Total number of leaves:" << this->leavesCount <<endl;
         this->out_file << "Total number of internal nodes:" << this->internalNodeCount <<endl;
         this->out_file << "Total number of internal nodes and leaves:" << this->leavesCount +this->internalNodeCount <<endl;
         this->out_file << "Average string depth of internal nodes:" << averageStringDepths << endl;
         this->out_file << "Length of maximum repeating characters:" << this->deepestNode->strDepth<<endl;
         this->out_file << "Start Index of maximum repeating characters:" << this->deepestNode->pathlabel[0] << endl;
         this->out_file << "Deepest Node Path Label / Maximum Repeating Characters:";
         displayTextInOutputWithCharLimitInLine(this->deepestNode->pathlabel[0],this->deepestNode->pathlabel[1]- this->deepestNode->pathlabel[0] +1, 100);
         this->out_file << "\n===========================================\n";

      }

      /* Display the children information of a given internal node. */
      void SuffixTree::displayChildren(Node *currentNode){
         if (currentNode->isLeaf){
            this->out_file << "The given node is a leaf node and has no children.";

            return;
         }

         for(Node *child: currentNode->children){
            this->out_file << "==========================================" << endl;
            this->out_file << "Node Id :" << child->id << endl;
            this->out_file << "String Depth:" << child->strDepth << endl;
            this->out_file << "Is Internal Node" << !child->isLeaf << endl;
            this->out_file << "Path Label:";
            displayTextInOutputWithCharLimitInLine(child->pathlabel[0],child->pathlabel[1] - child->pathlabel[0] + 1, 100);
            this->out_file << "\n";
         }
      }

      /* Gets the BWT Index for the Suffix Tree. */
      void SuffixTree::getBWTIndex(){
         DFSBWT(this->tree->root);
         this->bwt_output.close();
      }

