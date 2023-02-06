#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <limits>
#include <algorithm>
#include <tuple>
#include <cmath>


using namespace std;

struct DP_cell{
    int Sscore;
    int Dscore;
    int Iscore;
};


class Alignment{
    private:
        string sequence_1;
        string sequence_2;

        int match_score;

        int global;
        int score;

        signed int gap_score;
        signed int mismatch_score;
        signed int affinity_score;

        vector<vector<DP_cell>> table;

        ofstream out_file;

        // Reads the sequence input from the given filename.
        void read_input_file(string filename){

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

            this->sequence_1 = sequences[0];
            this->sequence_2 = sequences[1];
            indata.close();
            return ;
        };

        // Gives the maximum value for the provided three values. Used to find max while filling the 
        // Dp table.
        int getMaxValue(int value_1 , int value_2 ,int value_3){
            if (this->global == 1) {
                return max(max(value_1,value_2),value_3);
            }
            int value_4 = 0;
            return max(max(max(value_1,value_2),value_3),value_4);
        }

        // Gives the maximum value in the table
        tuple<int,int> get_overallMax(){
            int max = 0,end_index_i = 0,end_index_j = 0,i = 0,j = 0;
            for(vector<DP_cell> v1 : this->table){
                j = 0;
                for(DP_cell t : v1){
                    int temp = getMaxValue(t.Sscore,t.Dscore,t.Iscore);
                    if (temp >= max) {
                        max = temp;
                        end_index_i = i;
                        end_index_j = j;
                    }
                    j++;       
                }
                i++;               
            }
            return make_tuple(end_index_i,end_index_j);
        }

        /* Returns the type with the corresponding max values. Used to find which is the next case i.e D, I ,S while
        retracing. */
        char getMaxValueType(vector<int> comparables, vector<char> types){
            int max_index = max_element(comparables.begin(), comparables.end()) - comparables.begin();
    
            return types[max_index];
        }

        /* Function to calculate the S score while filling the table. */
        int getSscore(int index_i , int index_j){
            int prev_Sscore = this->table[index_i - 1][index_j -1].Sscore;
            int prev_Dscore = this->table[index_i - 1][index_j -1].Dscore;
            int prev_Iscore = this->table[index_i - 1][index_j -1].Iscore;

            int score = getMaxValue(prev_Dscore,prev_Iscore,prev_Sscore);


            if (this->global == 0) {
                int zero = 0;
                if(sequence_1.at(index_i-1) == sequence_2.at(index_j-1)){
                    return max(score + match_score,zero);
                }
                return max(score + mismatch_score,zero);
            }
            if(sequence_1.at(index_i-1) == sequence_2.at(index_j-1)){
                return score + match_score;
            }

            return score + mismatch_score;

        }

        /* Function to find the previous best type if the current best type is S. Used for retracing. */
        char getSscorePreviousType(int index_i , int index_j){
            int prev_Sscore = this->table[index_i - 1][index_j -1].Sscore;
            int prev_Dscore = this->table[index_i - 1][index_j -1].Dscore;
            int prev_Iscore = this->table[index_i - 1][index_j -1].Iscore;

            vector<char> types{'d','i','s'};
            vector<int> values{prev_Dscore, prev_Iscore, prev_Sscore};

            char type = getMaxValueType(values,types);

            return type;
        }

        /* Function to calculate the D score while filling the table. */
        int getDscore(int index_i , int index_j){
            int prev_Sscore = this->table[index_i - 1][index_j].Sscore;
            int prev_Dscore = this->table[index_i - 1][index_j].Dscore;
            int prev_Iscore = this->table[index_i - 1][index_j].Iscore;

            int score = getMaxValue(prev_Dscore + gap_score,prev_Iscore + affinity_score + gap_score,prev_Sscore + affinity_score + gap_score);

            return score;
        }

        /* Function to find the previous best type if the current best type is D. Used for retracing. */        
        char getDscorePreviousType(int index_i , int index_j){
            int prev_Sscore = this->table[index_i - 1][index_j].Sscore;
            int prev_Dscore = this->table[index_i - 1][index_j].Dscore;
            int prev_Iscore = this->table[index_i - 1][index_j].Iscore;

            vector<char> types{'d','i','s'};
            vector<int> values{prev_Dscore + gap_score, prev_Iscore + affinity_score + gap_score, prev_Sscore + affinity_score + gap_score};


            char type = getMaxValueType(values, types);

            return type;
        }
        
        /* Function to calculate the I score while filling the table. */
        int getIscore(int index_i , int index_j){
            int prev_Sscore = this->table[index_i][index_j - 1].Sscore;
            int prev_Dscore = this->table[index_i][index_j - 1].Dscore;
            int prev_Iscore = this->table[index_i][index_j - 1].Iscore;


            int score = getMaxValue(prev_Iscore + gap_score, prev_Dscore + affinity_score + gap_score, prev_Sscore + affinity_score + gap_score);

            return score;
        }

        /* Function to find the previous best type if the current best type is I. Used for retracing. */   
        char getIscorePreviousType(int index_i , int index_j){
            int prev_Sscore = this->table[index_i][index_j - 1].Sscore;
            int prev_Dscore = this->table[index_i][index_j - 1].Dscore;
            int prev_Iscore = this->table[index_i][index_j - 1].Iscore;

            vector<char> types{'d','i','s'};
            vector<int> values{prev_Dscore + affinity_score + gap_score,prev_Iscore + gap_score ,prev_Sscore + affinity_score + gap_score};
            
            char type = getMaxValueType(values,types);

            return type;
        }

        /* Function gives the best previous case for the retracing path. */
        char getRetraceCaseType( int index_i , int index_j ,char current_case_type){
            char selected_type;

            if(current_case_type == 'd'){
                return getDscorePreviousType(index_i,index_j);
            }
            if(current_case_type == 'i'){
                return getIscorePreviousType(index_i,index_j);
            }

            return getSscorePreviousType(index_i,index_j);
        }

        /* Function that performs the retrace tasks. It is used a recursive fucntion. */
        void retrace(int index_i , int index_j, vector<char> *aligned_str_1 , vector<char> *aligned_str_2, char cell_max_type){

            if (this->global == 0) {
                if(getMaxValue(this->table[index_i][index_j].Dscore,this->table[index_i][index_j].Sscore,this->table[index_i][index_j].Iscore) == 0 ){
                    this->out_file << "Starting Index for alignment : "<< index_i << "  " << index_j << "\n" ;
                    return;

                }
            }

            if (this->global == 1) {
                if(this->table[index_i][index_j].Dscore == 0 && this->table[index_i][index_j].Sscore == 0 && this->table[index_i][index_j].Iscore == 0 ){
                    this->out_file << "Starting Index for alignment : "<< index_i << "  " << index_j << "\n" ;
                    return;
                }
            }

            char next_case_type;
            
            if(index_i == this->sequence_1.length() && index_j == this->sequence_2.length()){
                vector<char> types{'d','i','s'};
                vector<int> values{this->table[index_i][index_j].Dscore,this->table[index_i][index_j].Iscore,this->table[index_i][index_j].Sscore};

                cell_max_type = getMaxValueType(values,types);
            }

            next_case_type = getRetraceCaseType(index_i, index_j , cell_max_type);
            
            if(cell_max_type == 'd'){
                
                /* Using index_i - 1 here because sequence starts at the 1 index in the table rather than 0. So to fetch the
                correct character from string we need to subtract the current index by 1. */
                aligned_str_1->push_back(this->sequence_1.at(index_i-1));
                aligned_str_2->push_back('-');

                return retrace(index_i -1 ,index_j, aligned_str_1,aligned_str_2,next_case_type);

            }else if(cell_max_type == 'i'){

                aligned_str_1->push_back('-');
                aligned_str_2->push_back(this->sequence_2.at(index_j-1));

                return retrace(index_i ,index_j - 1, aligned_str_1,aligned_str_2,next_case_type);
            }else{

                aligned_str_1->push_back(this->sequence_1.at(index_i-1));
                aligned_str_2->push_back(this->sequence_2.at(index_j-1));

                return retrace(index_i - 1,index_j - 1, aligned_str_1,aligned_str_2,next_case_type);
            }

            return;
            
        }

    public:
        Alignment(string sequence_file, int global, int match_score , int mismatch_score , int gap_score, int affinity_score ){
            read_input_file(sequence_file);
            if(global == 1 ) {
                this->out_file.open(sequence_file + ".global.out",ios::app);
            } else if(global == 0 ) {
                this->out_file.open(sequence_file + ".local.out",ios::app);
            }
            this->out_file << "Sequence_1 : " << this->sequence_1 << "\n" << flush;
            this->out_file << "Sequence_2 : " << this->sequence_2 << "\n"<< flush;
            this->out_file << "<----------------------------------------->" << "\n" << flush;
            this->out_file << "Length of Sequence_1 : " <<  this->sequence_1.length() << "\n" << flush;
            this->out_file << "Length of Sequence_2 : " <<  this->sequence_2.length() << "\n" << flush;
            this->out_file << "<----------------------------------------->" << "\n" << "\n" << flush;
            this->match_score = match_score;
            this->mismatch_score = mismatch_score;
            this->gap_score = gap_score;
            this->affinity_score = affinity_score;
            this->global = global;
            vector<vector<DP_cell>> table(sequence_1.length() +1, vector<DP_cell>(sequence_2.length() +1));
            this->table = table;
        }

        /* Function that fills the DP table in the feed forward method. */
        void fillAndGetTable(){
            
            this->out_file <<  "\n" << "Started Forward  Computation\n" << flush;

            int infinity = numeric_limits<int>::infinity();

            this->table[0][0].Sscore = 0;
            this->table[0][0].Dscore = 0;
            this->table[0][0].Iscore = 0;

            for (int index_i = 1 ; index_i <= this->sequence_1.length() ;  index_i++ ){
                if (this->global == 1) {
                    this->table[index_i][0].Sscore = 99999 * -1;
                    this->table[index_i][0].Iscore = 99999 * -1;
                    this->table[index_i][0].Dscore =  affinity_score + index_i * gap_score;
                }
                if (this->global == 0) {
                    this->table[index_i][0].Sscore = 0;
                    this->table[index_i][0].Iscore = 0;
                    this->table[index_i][0].Dscore = 0;
                }                
            }

            for (int index_j = 1 ; index_j <= this->sequence_2.length() ;  index_j++ ){
                if (this->global == 1) {
                    this->table[0][index_j].Sscore = 99999 * -1;
                    this->table[0][index_j].Iscore = affinity_score + index_j * gap_score;
                    this->table[0][index_j].Dscore =  99999 * -1;
                }
                if (this->global == 0) {
                    this->table[0][index_j].Sscore = 0;
                    this->table[0][index_j].Iscore = 0;
                    this->table[0][index_j].Dscore = 0;
                }      
            }
            

            for (int index_1 = 1; index_1 <= this->sequence_1.length(); index_1++){
                for(int index_2 =1 ; index_2 <= this->sequence_2.length(); index_2++){
                    this->table[index_1][index_2].Sscore = getSscore(index_1,index_2);
                    this->table[index_1][index_2].Dscore = getDscore(index_1,index_2);
                    this->table[index_1][index_2].Iscore = getIscore(index_1,index_2);
                }
            
            }
            this->out_file  << "Finished Forward  Computation\n" << flush;

            return;
        }

        void print_table(){

            this->out_file << "\n" << "<-------------Printing table------------->" << "\n" << flush;

            for(vector<DP_cell> v1 : this->table){
                for(DP_cell t : v1){
                    this->out_file << "[" << t.Sscore << ", " << t.Dscore << ", " << t.Iscore << "] ";
                }
                this->out_file << "\n" << flush;
            }

            this->out_file << "<----------------------------------------->" << "\n" << "\n" << flush;
        }


        /* Retraces the alignment from the end of the table and displays the output in the desired format. */
        void retraceAndGetAlignment(){

            int end_index_i = 0;
            int end_index_j = 0;
            if (this->global == 0) {
                tie(end_index_i,end_index_j) = get_overallMax();
            }

            if (this->global == 1) {
                end_index_i = this->sequence_1.length();
                end_index_j = this->sequence_2.length();
            }
            this->score = getMaxValue(this->table[end_index_i][end_index_j].Dscore,this->table[end_index_i][end_index_j].Iscore,this->table[end_index_i][end_index_j].Sscore);
            this->out_file <<  "\n" << "<----------------------------------------->" << "\n" ;
            this->out_file << "End Index for alignment      : "<< end_index_i << " " << end_index_j << "\n" << flush;

            vector<char> aligned_string_1;
            vector<char> aligned_string_2;

            char type;

            retrace(end_index_i, end_index_j, &aligned_string_1, &aligned_string_2, type);
            this->out_file <<  "\n" << "----- Printing Alignment ----- "<<  "\n" <<  "\n";

            vector<char> match_signs;

            int total_score = 0;
            int extension_count = 0;
            int open_count = 0;
            int mismatch_count = 0;
            int match_count = 0;

            for(int i = aligned_string_1.size() - 1; i>= 0 ; i-- ){
                if(aligned_string_1[i] ==aligned_string_2[i]){
                    
                    match_signs.push_back('|');
                    total_score += this->match_score;
                    match_count +=1;

                }else if (aligned_string_1[i] !=aligned_string_2[i] && aligned_string_1[i] != '-' && aligned_string_2[i] != '-'){
                    
                    match_signs.push_back(' ');
                    total_score += this->mismatch_score;

                    mismatch_count +=1;
                }else {
                    match_signs.push_back(' ');
                }
            }

            for(int i = aligned_string_1.size() - 1; i>= 0 ; i-- ){

                if(i < aligned_string_1.size() && aligned_string_1[i] == '-' && aligned_string_1[i+1] != '-' ){
                    total_score = total_score + this->affinity_score + this->gap_score ;
                    open_count+=1;
                    extension_count+=1;
                }

                if(i < aligned_string_1.size() && aligned_string_1[i] == '-' && aligned_string_1[i+1] == '-' ){
                    total_score += this->gap_score;
                    extension_count+=1;
                }
            }


            for(int i = aligned_string_2.size() - 1; i>= 0 ; i-- ){

                if(i < aligned_string_2.size() && aligned_string_2[i] == '-' && aligned_string_2[i+1] != '-' ){
                    total_score = total_score + this->affinity_score + this->gap_score ;
                    open_count+=1;
                    extension_count+=1;
                }

                if(i < aligned_string_2.size() && aligned_string_2[i] == '-' && aligned_string_2[i+1] == '-' ){
                    total_score += this->gap_score;
                    extension_count+=1;
                }
            }

            this->out_file <<endl;
            int size = aligned_string_2.size();

            for(int i = 0; i < size ; i+=60 ){
                this->out_file << "String 1 : " ;
                
                for(int j = size -i - 1; ((j>= size -i -60) && (j >=0)) ; j-- ){
                    this->out_file << aligned_string_1[j];
                }

                this->out_file <<  "\n";
                this->out_file << "           " ;

                for(int j = i; ((j<= i+ 60 - 1) && (j < size)) ; j++ ){
                    this->out_file << match_signs[j];
                }

                this->out_file <<  "\n";
                this->out_file << "String 2 : " ;

                for(int j = size -i - 1; ((j>= size -i -60) && (j >=0)) ; j-- ){
                    this->out_file << aligned_string_2[j];
                }
                
                this->out_file <<  "\n";
            }

            this->out_file << "\n" << "\n" << "------- Printing Scores ------- " <<  "\n";
            this->out_file <<endl;
            this->out_file << "Table Score:" << this->score <<endl;
            this->out_file << "Global Optimal Score: " << total_score <<endl;
            this->out_file << "Match Count: " << match_count <<endl;
            this->out_file << "Mismatch Count: " << mismatch_count <<endl;
            this->out_file << "Opening gaps: " << open_count << endl;
            this->out_file << "Total gaps: " << extension_count <<endl;
            this->out_file << "Identities: " << match_count << "/" << size << " (" << round(match_count*100/size) << "%)" << endl;
            this->out_file << "Gaps: " << extension_count << "/" << size << " (" << round(extension_count*100/size) << "%)" << endl;

            this->out_file << "\n" << "\n" << "<----------------------------------------->" << "\n" ;
            this->out_file.close();
        }
};


