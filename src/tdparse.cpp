
/************************************************************
* File:   	tdparse.cpp										*
* Date:   	07/22/2014										*
* Author: 	Thomas Lesniak									*
* Company: 	West Monroe Partners, LLC						*
* 															*
*															*
* The purpose of this code is to parse and consolidate 		*
* output *.txt files from the Microsoft SQL Server util 	*
* tablediff. This tool should aid in the location and 		*
* analysis of bulk data discrepencies between tables.		*
*															*
*************************************************************/

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

// set up linked list structure
struct node {
	int pkey;
	node* next;
};

int main( int argc, char* argv[] ) {


	// argument usage checking: only except 2 or 3 arguments

	if(argc < 3 || argc > 4 ){
		cout << "\nusage: " << argv[0] <<" infile  outfile  (min_count)" << endl;
		cout << "\ninfile  : The tablediff output text file to parse.\n" <<
				  "outfile : The consolidated analysis of the table data differences, as a text file.\n" <<
				  "------------(Optional)------------\n" << 
			     "min_count: The minimum number of consecutive rows with " <<
				  "non-matching data to be included in the output.\nDefault=1, min_count>=1\n\n" <<
				  "Example: tdparse.exe source.txt parsed.txt 10\n" << endl;
		return 1;
	}

	// argument processing

	int min_count;

	ifstream infile(argv[1]);
	if(!infile.is_open()) {
		cout << "Error: Unable to read from file \"" << argv[1] << "\"\n" << endl;
		return 1;
	}

	ofstream outfile(argv[2]);
	if(!outfile.is_open()) {
		cout << "Error: Unable to open file \"" << argv[2] << "\" for writing.\n" << endl;
		return 1;
	}
	
	if(argc == 4)
		min_count = atoi(argv[3]);
	else min_count = 1;

	if(min_count < 1) {
		cout << "Error: invalid argument min_count must be greater or equal to 1\n"<<endl;
		return 1;
	} 


	// local variable declarations and initializations
	string line, mis, src, dest, header, primary_key;
	primary_key = "";

	node* mishead;
	node* srchead;
	node* desthead;
	
	mishead = new node;
	mishead->pkey = -2;
	mishead->next = NULL;
	
	srchead = new node;
	srchead->pkey = -2;
	srchead->next = NULL;
	
	desthead = new node;
	desthead->pkey = -2;
	desthead->next = NULL;
	
	node* curM = mishead;
	node* curS = srchead;
	node* curD = desthead;
	
	mis = "Mismatched:\n\t";
	src = "Source Only:\n\t";
	dest = "Destination Only:\n\t";

	// read and process the first 2 lines of the tablediff input file
	getline(infile,header);
	getline(infile,primary_key);
	if (header == "The replication table difference tool requires the comparison tables/views to have either a primary key, identity, rowguid or unique key column.")
	{
		cout << "tablediff output:\n";
		cout << "The replication table difference tool requires the comparison tables/views to have either a primary key, identity, rowguid or unique key column." << endl;
		return 0;
	}
	if (primary_key == "")
	{
		cout << "tdparse: Could not parse tablediff output. Is there a primary key for this table?\n";
		return 1;
	}
	// store the primary key or other unique column name 
	primary_key = primary_key.substr(4,primary_key.find("\t",4)-4);
	if (primary_key.find("requested operation") != -1) {
		primary_key = "";
	}
	// read in the entire tablediff output, storing the unique column in 
	// its respective linked list
	while(getline(infile, line)) {
		int found;
		if((found = line.find("Mismatch",0)) != -1) {
			int t = stoi(line.substr(found+9,line.find("\t", found+9)-9),NULL,0);
			curM->next = new node;
			curM->next->pkey = t;
			curM->next->next = NULL;
			curM=curM->next;					
		}
		else if((found = line.find("Src. Only",0)) != -1) {
			int t = stoi(line.substr(found+10,line.find("\t", found+10)-10),NULL,0);
			curS->next = new node;
			curS->next->pkey = t;
			curS->next->next = NULL;
			curS=curS->next;			
		}
		else if((found = line.find("Dest. Only",0)) != -1) {
			int t = stoi(line.substr(found+11,line.find("\t", found+11)-11),NULL,0);
			curD->next = new node;
			curD->next->pkey = t;
			curD->next->next = NULL;
			curD=curD->next;			
		}
	}

	// gather consecutive different rows that are mismatched
	curM = mishead->next;
	while(curM!=NULL) {
		int count = 1;
		string tmp = "" + to_string(curM->pkey);
		while(curM->next != NULL && curM->pkey == curM->next->pkey-1){
			curM = curM->next;
			count++;
		}
		if (count >= min_count && count > 1) {
			tmp += "-" + to_string(curM->pkey) + "\n\t";
			mis += tmp;
		}
		else if(count == 1 && min_count == 1) {
			mis += tmp + "\n\t";
		}

		curM = curM->next;
	}
	
	// repeat previous for source only and destination only
	curS = srchead->next;
	while(curS!=NULL) {
		int count = 1;
		string tmp = "" + to_string(curS->pkey);
		while(curS->next != NULL && curS->pkey == curS->next->pkey-1){
			curS = curS->next;
			count++;
		}
		if (count >= min_count && count > 1) {
			tmp += "-" + to_string(curS->pkey) + "\n\t";
			src += tmp;
		}
		else if (count == 1 && min_count == 1) {
			src += tmp + "\n\t";
		}

		curS = curS->next;
	}

	curD = desthead->next;
	while(curD!=NULL) {
		int count = 1;
		string tmp = "" + to_string(curD->pkey);
		while(curD->next != NULL && curD->pkey == curD->next->pkey-1){
			curD = curD->next;
			count++;
		}
		if (count >= min_count && count > 1) {
			tmp += "-" + to_string(curD->pkey) + "\n\t";
			dest += tmp;
		}
		else if (count == 1 && min_count == 1) {
			dest += tmp + "\n\t";
		}
		
		curD = curD->next;
	}
	// write parsed results to output file 
	outfile << "tablediff parser v1.0"<<endl;
	outfile << "Any keys with consecutive discrepancies less than " << min_count << " have been omitted.\n"<<endl;
	outfile << header << endl;
	if (primary_key != "") {
	outfile << "The primary key used in this analysis was \"" << primary_key << "\"." << endl;
	}
	outfile << "Please refer to the tablediff output or the actual tables for more details on any differences.\n" <<endl;
	outfile << mis << endl;
	outfile << src << endl;
	outfile << dest << endl;

	//cleanup
	curM = mishead;
	while(curM != NULL) {
		node* tmp = curM->next;
		delete curM;
		curM = tmp;
	}	
	curS = srchead;
	while(curS != NULL) {
		node* tmp = curS->next;
		delete curS;
		curS = tmp;
	}
	curD = desthead;
	while(curD != NULL) {
		node* tmp = curD->next;
		delete curD;
		curD = tmp;
	}
	infile.close();
	outfile.close();
	cout << "tdparse successful\n";
	return 0;
}

	
	
