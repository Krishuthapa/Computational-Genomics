#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>

#include "suffix_tree.h"

using namespace std;


// Reads and gives the sequence input from the given filename.
string read_sequence_input_file(string filename){

   ifstream indata;
   vector<string> sequences;
   indata.open(filename);
   
   if(!indata) {
      cerr << "Error: Unable to open the file" << endl;
      exit(1);
   }

   if (indata) { 
      string line;
      int new_sequence = -1;
      string sequence_segment = "";
      
      while (getline(indata, line)) { 
         if(line.empty()) { continue; }
               if(line[0] == '>') {
                  sequences.push_back(sequence_segment);
                  new_sequence++;
               } else {
                     sequences[new_sequence].append(line);
               }
         }
   }

   string sequence = sequences[0];

   indata.close();

   return sequence;
   };

int main(int argc, char** argv)
{

   if (argc < 1) {
      cout << "You have entered less than 1 arguments:" << "\n";
      cout << "Please enter in the mentioned format -->  filename <sequence_file>\n";
      exit(1);
   }

   string sequence_file = argv[1];

   ofstream out_file;
   
   out_file.open(sequence_file + ".out");

   out_file <<  "\n" << "------------- INPUTS -------------\n";
   out_file << "Sequence File:\t" << sequence_file << "\n";
   out_file << "-----------------------------------\n";

   string sequence = read_sequence_input_file(sequence_file);
   string input_string = sequence + '$';


   out_file << "String Length:" << input_string.length() << endl;

   /* The second argument is the sequence file name to create the output file with the same name as sequence. */
   SuffixTree tree(input_string, sequence_file);

   auto start = chrono::high_resolution_clock::now();
   tree.buildTree();
   auto end = chrono::high_resolution_clock::now();

   auto tree_building_duration = chrono::duration_cast<chrono::milliseconds>(end - start);
   out_file << "Running Time to build tree in milliseconds :" << tree_building_duration.count() << endl;

   tree.getStatistics();
   tree.getBWTIndex();

   //tree.implementDFSAndEnumerate();

   out_file.close();
   return 0;

}