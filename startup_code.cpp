#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iterator>
#include <algorithm>

// #include <random>


// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// allocates things on heap
vector<vector<int>> dataset;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{
public:
	string Node_Name;  // Variable name 
	vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning

// public:
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
	
		nvalues=n;
		values=vals;
		

	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
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
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
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
	

};

network Alarm;



template<typename T>
void printVector(const T& t) {
    cout << "[ ";
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, ", "));
    cout << "]" << endl;
} 


network read_network(){
	network Alarm1;
	string line;
	int find=0;
  	ifstream myfile("alarm.bif"); 
  	string temp;
  	string name;
  	vector<string> values;
  	
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
 				Graph_Node new_node(name,values.size(),values);
 				int pos=Alarm1.addNode(new_node);
     		}
     		else if(temp.compare("probability")==0){
 				ss>>temp;
 				ss>>temp;
 				
                list<Graph_Node>::iterator listIt;
                list<Graph_Node>::iterator listIt1;
 				listIt=Alarm1.search_node(temp);
                int index=Alarm1.get_index(temp);
                ss>>temp;
                values.clear();
 				while(temp.compare(")")!=0){
                    listIt1=Alarm1.search_node(temp);
                    listIt1->add_child(index);
 					values.push_back(temp);
 					ss >> temp;
				}
                listIt->set_Parents(values);
				getline (myfile,line);
 				stringstream ss2;
 				ss2.str(line);
 				ss2>> temp;
 				ss2>> temp;
 				vector<float> curr_CPT;
                string::size_type sz;
 				while(temp.compare(";")!=0){
                    // curr_CPT.push_back(atof(temp.c_str()));
 					float random_num = ((float)(rand()%100) +1)/101.0;
                    curr_CPT.push_back(random_num);
 					ss2>>temp;
				}
                listIt->set_CPT(curr_CPT);
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

void read_dataset(int num_vars, vector<int> *indexes){
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
            }
            else{
                vector<string>::iterator valuesIT;
                int ind = 0;
                for(valuesIT=gg->values.begin(); valuesIT<gg->values.end(); valuesIT++){
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
        if(!found_q){
            indexes->push_back(-1);
        }
        dataset.push_back(vec);
    }
}



int main()
{
	Alarm=read_network();
    
    // Example: to do something
	cout<<"Perfect! Hurrah! \n" << endl;
	
    list<Graph_Node>::iterator listIt;
    // for(listIt=Alarm.Pres_Graph.begin();listIt!=Alarm.Pres_Graph.end();listIt++){
    //     printVector(listIt->get_values());
    //     // cout << listIt->get_Parents().size() << " " << endl;
    // }

    vector<int> q_indexes;
    read_dataset(Alarm.Pres_Graph.size(), &q_indexes);

}





