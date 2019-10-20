#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <iomanip>


// #include <random>


#define THRESHOLD 0.0001

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// allocates things on heap
vector<vector<int>> dataset;
ofstream outfile;


template<typename T>
void printVector(const T& t) {
    // cout << "[ ";
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, " "));
    // cout << "]" ;
} 



// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{
public:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<int> Parents; // Parents of a particular node- note these are names of parents
    vector<string> Parents_names;
    vector<int> Parents_nvalues;
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    vector<int> Table;
    int my_index;

// public:
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals, int ind)
	{
		Node_Name=name;
		nvalues=n;
		values=vals;
        my_index = ind;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT){
		CPT.clear();
		CPT=new_CPT;
	}

    void extra_work(int size){
        // create a table for keeping counts for CPT
        this->Table.clear();
        cout << "here ";
        // for(int i=0; i<size; i++){
        //     this->Table.push_back(0);
        // }
        vector<int> vec(size, 0);
        Table = vec;
        cout << "there" << endl;
    }

    void set_Parents(vector<int> Parent_Nodes, vector<int> nvals_parent, vector<string> Pnames)
    {
        // Parents.clear();
        Parents=Parent_Nodes;
        // Parents_nvalues.clear();
        Parents_nvalues=nvals_parent;
        // Parents_names.clear();
        Parents_names=Pnames;

        // cout << "----------------- \n";
        // printVector(Parents);
        // printVector(Parents_nvalues);
        // cout << endl << endl;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        // for(int i=0;i<Children.size();i++)
        // {
        //     if(Children[i]==new_child_index)
        //         return 0;
        // }
        Children.push_back(new_child_index);
        return 1;
    }

    void fill_count_table(vector<vector<int>>::iterator example){
        int index = 0;
        int group_size = 1;
        for(int i=Parents.size()-1; i>=0; i--){
            // this parents value is (*example)[Parents[i]]
            index += group_size * ((*example)[Parents[i]]);
            // group_size *= graph.get_nth_node(Parents[i])->nvalues;
            group_size *= Parents_nvalues[i];
        }
        index += group_size * ((*example)[this->my_index]);
        Table[index] = this->Table[index] + 1 ;
    }

    bool update_CPT(){
        bool unchanged = true;
        int n_cols = Table.size()/nvalues; 
        for(int j=0; j<Table.size()/nvalues; j++){
            int norm = 0;
            bool no_zero_exists = true;
            for(int i=0; i<nvalues; i++){
                norm += Table[i*n_cols + j];
                no_zero_exists = no_zero_exists && (Table[i*n_cols + j] != 0);
            }
            float smoothing_factor = 0;
            if(!no_zero_exists) smoothing_factor = ((float)norm)/100.0;
            if(norm==0) smoothing_factor += 0.0001;
            for(int i=0; i<nvalues; i++){
                int ind = i*n_cols+ j;
                float prev = CPT[ind];
                CPT[ind] = (((float)Table[ind])+smoothing_factor)/((float)norm + ((float)nvalues)*smoothing_factor);
                if(CPT[ind]==0) cout << Node_Name << ":\t[" << ind << "]:" <<CPT[ind] << "\tnorm: " << norm << "\tmy_val: " << Table[ind] << endl;
                unchanged = unchanged && (abs(prev-CPT[ind]) < THRESHOLD);
                Table[ind] = 0;
            }
        }
        return unchanged;
    }

    float prob_me(vector<vector<int>>::iterator example){
        int index = 0;
        int group_size = 1;
        for(int i=Parents.size()-1; i>=0; i--){
            index += group_size * ((*example)[Parents[i]]);
            group_size *= Parents_nvalues[i];
        }
        index += group_size * ((*example)[my_index]);
        return CPT[index];
    }

    int infer(vector<vector<int>>::iterator example, auto graph){
        // TODO: CHECK: also set the "?" with the inferred value
        int prev = (*example)[my_index];
        float max = -1;
        int inferred_val = -1;

        // iterate over all values and find one with max prob. 
        for(int val=0; val<nvalues; val++){
            (*example)[my_index] = val;
            float prob = 1;
            prob *= prob_me(example);
            // find prob for each child being as in example when you have value=val
            for(int i=0; i<Children.size(); i++){
                prob *= graph->get_nth_node(Children[i])->prob_me(example);
            }
            if(prob > max){
                max = prob;
                inferred_val = val;
            }
        } 

        (*example)[my_index] = inferred_val;
        return inferred_val;
    }
};

 // The whole network represted as a list of nodes
class network{
public:
	list <Graph_Node> Pres_Graph;
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->Node_Name.compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
    // get the node at nth index
    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->Node_Name.compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }

    pair<int, int> search_node_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int index = 0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->Node_Name.compare(val_name)==0)
                return pair<int, int>(index, listIt->nvalues);
            index ++;
        }

            cout<<"node not found\n";
        return pair<int, int>(-1, -1);
    }    
	

};


network Alarm;



network read_network(){
	network Alarm1;
	string line;
	int find=0;
  	ifstream myfile("alarm.bif"); 
  	string temp;
  	string name;
  	vector<string> values;
    vector<int> values2;
    vector<int> values3;
    vector<string> values4;
  	int num_nodes = 0;
    bool done_print1 = false;

    if (myfile.is_open()){
    	while (! myfile.eof() ){
    		stringstream ss;
      		getline (myfile,line);
      		

      		ss.str(line);
     		ss>>temp;

            if(!done_print1 && temp.compare("probability")!=0) outfile << line << endl;

     		
     		if(temp.compare("variable")==0){
 				ss>>name;
 				getline (myfile,line);
                if(!done_print1) outfile << line << endl;

 				stringstream ss2;
 				ss2.str(line);
 				for(int i=0;i<4;i++){
 					ss2>>temp;
				}
 				values.clear();
 				while(temp.compare("};")!=0)
 				{
 					values.push_back(temp);
 					
 					ss2>>temp;
				}
 				Graph_Node new_node(name,values.size(),values, num_nodes);
 				Alarm1.Pres_Graph.push_back(new_node);
                num_nodes++;
     		}
     		else if(temp.compare("probability")==0){
                done_print1 = true;
 				ss>>temp;
 				ss>>temp;
 				
                list<Graph_Node>::iterator listIt;
                list<Graph_Node>::iterator listIt1;
 				listIt=Alarm1.search_node(temp);
                int index=Alarm1.get_index(temp);
                ss>>temp;
                values2.clear();
                values3.clear();
                values4.clear();
 				while(temp.compare(")")!=0){
                    listIt1=Alarm1.search_node(temp);
                    listIt1->add_child(index);
                    pair<int, int> ind = Alarm1.search_node_index(temp);
 					values2.push_back(ind.first);
                    values3.push_back(ind.second);
                    values4.push_back(temp);
 					ss >> temp;
				}
                listIt->set_Parents(values2, values3, values4);
				getline (myfile,line);
 				stringstream ss2;
 				ss2.str(line);
 				ss2>> temp;
 				ss2>> temp;
 				vector<float> curr_CPT;
                string::size_type sz;
 				while(temp.compare(";")!=0){
                    // curr_CPT.push_back(atof(temp.c_str()));
 					float random_val = ((float)(rand()%100) +1)/101.0;
                    curr_CPT.push_back(random_val);
 					ss2>>temp;
				}
                listIt->set_CPT(curr_CPT);
                listIt->extra_work(curr_CPT.size());
     		}
            else{
                // NOTHING
            }
    	}

    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm1;
}

void print_CPT(){
    for(auto iter=Alarm.Pres_Graph.begin(); iter!=Alarm.Pres_Graph.end(); iter++){
        cout << iter->Node_Name << endl;
        printVector(iter->CPT);
        cout << endl;
        cout << endl;
    }
}

void read_dataset(int num_vars, vector<int> *indexes, vector<int> *assignments){
    string line;
    ifstream myfile("records.dat");
    // int line_no = 0;
    while(!myfile.eof()){
        // cout << line_no << endl;
        stringstream ss;
        vector<int> vec(num_vars);
        string temp;
        getline(myfile, line);
        ss.str(line);
        bool found_q = false;
        list<Graph_Node>::iterator gg = Alarm.Pres_Graph.begin();
        for(int i=0; i<num_vars; i++){
            ss >> temp;
            if(temp.compare("\"?\"")==0){
                found_q = true;
                indexes->push_back(i);
                int rand_val = rand()%gg->nvalues;
                // rand_val = -1;
                assignments->push_back(rand_val);
                vec[i] = rand_val;
            }
            else{
                vector<string>::iterator valuesIT;
                int ind = 0;
                for(valuesIT=gg->values.begin(); valuesIT!=gg->values.end(); valuesIT++){
                    if(valuesIT->compare(temp)==0){
                        vec[i] = ind;
                        break;
                    }
                    // TODO: remove this (not needed) (just for debugging)
                    if(ind==gg->nvalues-1){
                        cout << "ERROR: called find with [" << temp << "] and values ";
                        printVector(gg->get_values());
                    }
                    ind++;
                }
            }
            std::advance(gg, 1);        
        }
        // TODO: USED ONLY IF USING EM2. Then can skip step1 of EM2
        // for(gg = Alarm.Pres_Graph.begin(); gg!=Alarm.Pres_Graph.end(); gg++){
        //     gg->fill_count_table(vec); 
        //      THIS WON'T work directly now. Changed fill_count_table to accept iterator of vector, but not the vector.
        // } 

        if(!found_q){
            indexes->push_back(-1);
            assignments->push_back(-1);
        }
        dataset.push_back(vec);
    }
}

void EM(vector<int>& q_indexes){
    // cout << "Called EM" << endl;
    bool unchanged = false;
    int num_iterations = 0;
    vector<vector<int>>::iterator iter_data;
    vector<int>::iterator q_iter;
    list<Graph_Node>::iterator nodes_iter;
    list<Graph_Node>::iterator nodes_iter2;

    // cout << "starting EM" << endl;
    // utill convergence
    while((!unchanged) && (num_iterations!=3000)){
        // cout << "iter: " << num_iterations << endl;
        unchanged = true;
        // 1.1 iterate over dataset, infer "?" 
        // 1.2 add counts to table
        int ex_num = 0;

        q_iter = q_indexes.begin();
        for(iter_data=dataset.begin(); iter_data!=dataset.end(); iter_data++){
            if((*q_iter)!=-1){
                Alarm.get_nth_node((*q_iter))->infer(iter_data, &Alarm);
            }
            // add counts to all nodes
            for(nodes_iter = Alarm.Pres_Graph.begin(); nodes_iter!=Alarm.Pres_Graph.end(); nodes_iter++){
                nodes_iter->fill_count_table(iter_data);
            }
            q_iter++;
            ex_num++;
        }
        // 2. update CPT using tables for each node
        for(nodes_iter = Alarm.Pres_Graph.begin(); nodes_iter!=Alarm.Pres_Graph.end(); nodes_iter++){
            // printVector(nodes_iter->Table);
            // cout << endl;
            unchanged = (nodes_iter->update_CPT() && unchanged);
            // printVector(nodes_iter->Table);
            // cout << endl;
        }

        num_iterations++;
        // if(num_iterations%1 == 0){
        //     cout << num_iterations << " iterations of EM over" << endl;
        // }
        // if(unchanged)
        //     cout << "CONVERGED" << endl;
    }
    return;
}

// void EM2(){
//     // if first time fill_table (Can skip using fill_table in read_dataset. May then need to clean tables.)
//     // else update_table
// }

void print_bif(){
    list<Graph_Node>::iterator iter;
    for(iter = Alarm.Pres_Graph.begin(); iter != Alarm.Pres_Graph.end(); iter++){
        outfile << "probability (  " << iter->Node_Name;
        for(vector<string>::iterator i=iter->Parents_names.begin(); i!=iter->Parents_names.end(); i++){
            outfile << "  " << (*i);
        }
        outfile << " ) { //" << (iter->Parents.size()+1) << " variable(s) and " << iter->CPT.size() << " values\n";
        outfile << "\ttable ";
        for(auto i=iter->CPT.begin(); i!=iter->CPT.end(); i++){
            outfile << fixed<< setprecision(4)<<(*i) << " ";
        }
        outfile << ";" << endl << "}\n";
    }
}


int main()
{
    outfile.open("solved_alarm.bif");
	Alarm=read_network();

    // Example: to do something
	// cout<<"Perfect! Hurrah! \n" << endl;
	
    list<Graph_Node>::iterator listIt;
    // for(listIt=Alarm.Pres_Graph.begin();listIt!=Alarm.Pres_Graph.end();listIt++){
    //     // printVector(listIt->get_Parents());
    //     // cout << accumulate(listIt->Table.begin(), listIt->Table.end(), 0)<< "\t";
    //     cout << listIt->my_index << ": \t" << listIt->nvalues << "\tParents: "; 
    //     printVector(listIt->Parents);
    //     cout << "\t" << "Parents_nvalues: ";
    //     printVector(listIt->Parents_nvalues);
    //     cout << endl;

    // }

    vector<int> q_indexes;
    vector<int> assignments;
    read_dataset(Alarm.Pres_Graph.size(), &q_indexes, &assignments);
    
    // cout << "Dataset read successfully" << endl;


    // for(auto iter_data=dataset.begin(); iter_data!=dataset.end(); iter_data++){
    //     printVector(*iter_data);
    //     cout << endl;
    // }



    EM(q_indexes);
    // return 0;

    // cout << "EM is over" << endl;
    // dataset.clear();
    // q_indexes.clear();
    // assignments.clear();

    // cout << "dataset size is " << dataset.size() << endl;
    print_CPT();
    print_bif();

    return 0;
}





