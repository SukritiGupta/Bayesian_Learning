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

// #include <random>


#define THRESHOLD 0.0001

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// allocates things on heap
vector<vector<int>> dataset;


template<typename T>
void printVector(const T& t) {
    cout << "[ ";
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
    cout << "]" << endl;
} 



// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{
public:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<int> Parents; // Parents of a particular node- note these are names of parents
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
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<int> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
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

    void set_Parents(vector<int> Parent_Nodes, vector<int> nvals_parent)
    {
        Parents.clear();
        Parents=Parent_Nodes;
        Parents_nvalues.clear();
        Parents_nvalues=nvals_parent;

        cout << "----------------- \n";
        printVector(Parents);
        printVector(Parents_nvalues);
        cout << endl << endl;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }

    void fill_count_table(vector<vector<int>>::iterator example, auto graph){
        // printf("called");
        int index = 0;
        int group_size = 1;
        for(int i=Parents.size()-1; i>=0; i--){
            // this parents value is (*example)[i]
            index += group_size * ((*example)[Parents[i]]);
            // group_size *= graph.get_nth_node(Parents[i])->nvalues;
            group_size *= Parents_nvalues[i];
        }
        index += group_size * ((*example)[this->my_index]);
        Table[index] = this->Table[index] + 1 ;
        // printf(" and returned\n");
    }

    void update_count_table(vector<int>& example, int changed_variable, int last_value){
        cout << "ERROR: Method \"update_count_table\" not implemented yet\n";
        return;
    }

    bool update_CPT(){
        // cout << "called";
        bool unchanged = true;
        float normalisation = accumulate(this->Table.begin(), this->Table.end(), 0.0);
        for(int i=0; i<this->CPT.size(); i++){
            float prev = this->CPT[i];
            this->CPT[i] = Table[i]/normalisation;
            unchanged = unchanged && (prev-CPT[i] < THRESHOLD); 
        }
        for(int i=0; i<this->CPT.size(); i++){
            this->Table[i] = 0;
        }
        // Table.clear();
        // vector<int> vec(CPT.size(), 0);
        // Table = vec;
        // cout << " and returned\n";
        return unchanged;
    }

    int infer(vector<vector<int>>::iterator example){
        // TODO: CHECK: also set the "?" with the inferred value
        (*example)[this->my_index] = 0;
        return 0;
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
            if(listIt->get_name().compare(val_name)==0)
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
            if(listIt->get_name().compare(val_name)==0)
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
            if(listIt->get_name().compare(val_name)==0)
                // (*to_set_nvalues) = listIt->nvalues;
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
  	int num_nodes = 0;

    if (myfile.is_open()){
    	while (! myfile.eof() ){
    		stringstream ss;
      		getline (myfile,line);
      		
      		ss.str(line);
     		ss>>temp;
     		
     		if(temp.compare("variable")==0){
 				ss>>name;
 				getline (myfile,line);
               
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
 				int pos=Alarm1.addNode(new_node);
                num_nodes++;
     		}
     		else if(temp.compare("probability")==0){
 				ss>>temp;
 				ss>>temp;
 				
                list<Graph_Node>::iterator listIt;
                list<Graph_Node>::iterator listIt1;
 				listIt=Alarm1.search_node(temp);
                int index=Alarm1.get_index(temp);
                ss>>temp;
                values2.clear();
                values3.clear();
 				while(temp.compare(")")!=0){
                    listIt1=Alarm1.search_node(temp);
                    listIt1->add_child(index);
                    pair<int, int> ind = Alarm1.search_node_index(temp);
 					values2.push_back(ind.first);
                    values3.push_back(ind.second);
 					ss >> temp;
				}
                listIt->set_Parents(values2, values3);
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

void read_dataset(int num_vars, vector<int> *indexes, vector<int> *assignments){
    string line;
    ifstream myfile("records.dat");
    while(!myfile.eof()){
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
                assignments->push_back(rand_val);
                vec[i] = rand_val;
            }
            else{
                vector<string>::iterator valuesIT;
                int ind = 0;
                for(valuesIT=gg->values.begin(); valuesIT!=gg->values.end(); valuesIT++){
                    if(valuesIT->compare(temp)==0){
                        vec[i] = ind;
                        // cout << "READ [" << temp << "] as " << vec[i] << " from values";
                        // printVector(gg->get_values());  
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
    cout << "Called EM" << endl;
    bool unchanged = false;
    int num_iterations = 0;
    vector<vector<int>>::iterator iter_data;
    vector<int>::iterator q_iter;
    list<Graph_Node>::iterator nodes_iter;
    list<Graph_Node>::iterator nodes_iter2;

    cout << "starting EM" << endl;
    // utill convergence
    while((!unchanged) && (num_iterations!=3000)){
        cout << "iter: " << num_iterations << endl;
        unchanged = true;
        // 1.1 iterate over dataset, infer "?" 
        // 1.2 add counts to table
        int ex_num = 0;

        q_iter = q_indexes.begin();
        for(iter_data=dataset.begin(); iter_data!=dataset.end(); iter_data++){
            if((*q_iter)!=-1){
                Alarm.get_nth_node((*q_iter))->infer(iter_data);
            }
            // add counts to all nodes
            for(nodes_iter = Alarm.Pres_Graph.begin(); nodes_iter!=Alarm.Pres_Graph.end(); nodes_iter++){
                nodes_iter->fill_count_table(iter_data, Alarm);
            }
            q_iter++;
            ex_num++;
        }
        cout << "hehe" << endl;
        // 2. update CPT using tables for each node
        for(nodes_iter = Alarm.Pres_Graph.begin(); nodes_iter!=Alarm.Pres_Graph.end(); nodes_iter++){
            unchanged = unchanged && nodes_iter->update_CPT();
        }

        num_iterations++;
        if(num_iterations%1 == 0){
            cout << num_iterations << " iterations of EM over" << endl;
        }
        if(unchanged)
            cout << "CONVERGED" << endl;
    }
    return;
}

// void EM2(){
//     // if first time fill_table (Can skip using fill_table in read_dataset. May then need to clean tables.)
//     // else update_table
// }


int main()
{
	Alarm=read_network();
    
    // Example: to do something
	cout<<"Perfect! Hurrah! \n" << endl;
	
    list<Graph_Node>::iterator listIt;
    // for(listIt=Alarm.Pres_Graph.begin();listIt!=Alarm.Pres_Graph.end();listIt++){
    //     printVector(listIt->get_Parents());
    //     // cout << listIt->get_Parents().size() << " " << endl;
    // }

    vector<int> q_indexes;
    vector<int> assignments;
    read_dataset(Alarm.Pres_Graph.size(), &q_indexes, &assignments);
    
    cout << "Dataset read successfully" << endl;

    EM(q_indexes);

    cout << "EM is over" << endl;
    dataset.clear();
    q_indexes.clear();
    assignments.clear();

    cout << "dataset size is " << dataset.size() << endl;

    return 0;
}





