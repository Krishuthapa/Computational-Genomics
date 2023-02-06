#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <alignment.cpp>

using namespace std;



int main(int argc, char** argv)
{


   if (argc < 3) {
      cout << "You have entered less than 2 arguments:" << "\n";
      cout << "Please enter in the mentioned format -->  filename <sequence_file> <0/1 - global/local> <optional:parameters_file>\n";
      exit(1);
   }

   string sequence_file = argv[1];
   int align_choice = atoi (argv[2]);
   string parameters_file;

   if(argc > 3) {
      parameters_file = argv[3];
   }
   else{
      string paramsFilename = "parameters.config";
   }

   ofstream out_file;
   if(align_choice == 0 ) {
      out_file.open(sequence_file + ".global.out");
   } else if(align_choice == 1 ) {
      out_file.open(sequence_file + ".local.out");
   } else{
      cerr << "Error: Invalid input for alignment " << endl;
      exit(1);
   }

   out_file <<  "\n" << "------------- INPUTS -------------\n";
   out_file << "Sequence File:\t" << sequence_file << "\n";
   out_file << "Global/Local:\t" << align_choice << "\n";
   out_file << "Parameters Config File:\t" << parameters_file << "\n";
   out_file << "-----------------------------------\n";

   int match_score, mismatch_score, gap_score, affinity_score;
   
   ifstream indata;
   indata.open(parameters_file);
   if(!indata) {
      cerr << "Error: Unable to open the file" << endl;
      exit(1);
   }

   string name;
   int var1;
   while (  indata >> name >> var1 ) { 
      if(name == "match"){
         match_score = var1;
      }
      if(name == "mismatch"){
         mismatch_score = var1;
      }
      if(name == "h"){
         affinity_score = var1;
      }
      if(name == "g"){
         gap_score = var1;
      }

   }

   out_file << "match: " << match_score << "\n";
   out_file << "mismatch: " << mismatch_score << "\n";
   out_file << "h: " << affinity_score << "\n";
   out_file << "g: " << gap_score << "\n";
   out_file << "-----------------------------------\n";
   indata.close();
   //string sequence_1 = "acaagtgatc" ;
   //string sequence_2 = "acagagtaac" ;

   if (align_choice == 0) {
      out_file << "Starting Global Alignment : " <<  "\n";
      Alignment alignment_g(sequence_file, 1, match_score , mismatch_score, gap_score, affinity_score);
      alignment_g.fillAndGetTable();
      //alignment_g.print_table();
      alignment_g.retraceAndGetAlignment();  
   } else if (align_choice == 1) {
      out_file << "Starting Local Alignment : " <<  "\n";
      Alignment alignment_g(sequence_file, 0, match_score , mismatch_score, gap_score, affinity_score);
      alignment_g.fillAndGetTable();
      //alignment_g.print_table();
      alignment_g.retraceAndGetAlignment();   
   } else {
      cerr << "Error: Invalid choice of alignment. Valid options are <0/1 - global/local> " << endl;
      exit(1);
   }
   out_file.close();
   return 0;

}
