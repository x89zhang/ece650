#include <memory>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <regex>
#include <algorithm>
class v_cover{
    public:
    bool is_cover;
    std::vector<unsigned> vers;
};
v_cover reduction(std::vector<unsigned> edge, int V, int k){
    //ref ece650-minisat.cpp
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    std::vector<std::vector<Minisat::Lit>> atom(V+1,std::vector<Minisat::Lit>(k+1));
    for (int i=1;i<V+1;i++){
        for (int j=1;j<k+1;j++){
            atom[i][j]=Minisat::mkLit(solver->newVar());
        }
    }
    //first kind of clause
    
    Minisat::vec<Minisat::Lit> clause_temp1;
    for (int i=1;i<k+1;i++){
        for (int j=1;j<V+1;j++){
            clause_temp1.push(atom[j][i]);
        }
        //clause1.push_back(clause_temp1);
        solver->addClause(clause_temp1);
        clause_temp1.clear();
    }
    //second kind of clause
    if (k>1){
    
    Minisat::vec<Minisat::Lit> clause_temp2;
    for (int m=1;m<V+1;m++){
        for (int p=1;p<k+1;p++){
            for (int q=p+1;q<k+1;q++){
                clause_temp2.push(~atom[m][p]);
                clause_temp2.push(~atom[m][q]);
                //clause2.push_back(clause_temp2);
                solver->addClause(clause_temp2);
                clause_temp2.clear();
            }
        }
    }
    }
    //third
    
    Minisat::vec<Minisat::Lit> clause_temp3;
    for (int m=1;m<k+1;m++){
        for (int p=1;p<V+1;p++){
            for (int q=p+1;q<V+1;q++){
                clause_temp3.push(~atom[p][m]);
                clause_temp3.push(~atom[q][m]);
                //clause3.push_back(clause_temp3);
                solver->addClause(clause_temp3);
                clause_temp3.clear();
            }
        }
    }
    //fourth
    
    Minisat::vec<Minisat::Lit> clause_temp4;
    for (int i=0;i<edge.size();i+=2){
        for (int m=1;m<k+1;m++){
            clause_temp4.push(atom[edge[i]][m]);
            clause_temp4.push(atom[edge[i+1]][m]);
        }
        //clause4.push_back(clause_temp4);
        solver->addClause(clause_temp4);
        clause_temp4.clear();
    }
    v_cover res;
    res.is_cover=solver->solve();
    if (!res.is_cover){
        res.vers.clear();
        return res;
    }
    for (int i=1;i<V+1;i++){
        for (int j=1;j<k+1;j++){
            if(Minisat::toInt(solver->modelValue(atom[i][j]))==0){
                res.vers.push_back(i);
                
            }

        }
    }
    return res;
}
//code in ece650-a2.cpp
class graph{
public:
    unsigned num_ver;
    std::vector<unsigned> edge_vec; //edge_vec is in the form of {1,2,3,4,5,6,7,8,9,10,11,12}
    //Applying BFS to search for the shortest path
    std::vector<int> shortest_path(unsigned start,unsigned end) const{
        std::queue<unsigned> bfs_queue;
        std::vector<unsigned> visited;
        std::vector<int> path;
        std::vector<int> path_vec;
        //due to vertex 0 not exist
        for (size_t i0=0; i0<num_ver+1;i0++){
            path.push_back(-1);
        }

        bfs_queue.push(start);
        while (!bfs_queue.empty()) {
            int current=bfs_queue.front();
            bfs_queue.pop();
            visited.push_back(current);
            
            for (size_t i1=0;i1<edge_vec.size();i1++){
                if (edge_vec[i1]==current){
                    if (i1%2==0){
                        int visit1=0;
                        for (size_t i2=0;i2<visited.size();i2++){
                            if (visited[i2]==edge_vec[i1+1]){
                                visit1=1;
                                break;
                            }
                        }
                        if (visit1==0) {
                            bfs_queue.push(edge_vec[i1+1]);
                            visited.push_back(edge_vec[i1+1]);
                            path[edge_vec[i1+1]]=current;
                        }
                        
                    } else {
                        int visit2=0;
                        for (size_t i3=0; i3<visited.size();i3++){
                            if (visited[i3]==edge_vec[i1-1]){
                                visit2=1;
                                break;
                            }
                        }
                        if (visit2==0) {
                            bfs_queue.push(edge_vec[i1-1]);
                            visited.push_back(edge_vec[i1-1]);
                            path[edge_vec[i1-1]]=current;
                        }
                        
                    }
                    
                }
                
            }
        }
        path_vec.push_back(end);
        while (path_vec.back()!=start){
            if (path_vec.back()==-1){
                reverse(path_vec.begin(),path_vec.end());
                return path_vec;
            }
            path_vec.push_back(path[path_vec.back()]);
        }
        reverse(path_vec.begin(),path_vec.end());
        return path_vec;


    }



};


int main(int argc, char** argv) {
    
    graph gg;
    //int is_V=0;
    int is_E=0;
    //int is_s=0;
    gg.num_ver=0;

    // read from stdin until EOF
    //using the example code in ece650-a2.cpp file to parse the input
    while (!std::cin.eof()) {
        

        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);
        if(line.empty()){
            continue;
        }

        // create an input stream based on the line
        // we will use the input stream to parse the line
        std::istringstream input(line);

        // we expect each line to contain a list of numbers
        // this vector will store the numbers.
        // they are assumed to be unsigned (i.e., positive)
        std::vector<unsigned> nums;
        
        std::string command1;
        
        command1=line.substr(0,2);
        
        // while there are characters in the input line
        while (!input.eof()) {
            unsigned num;
            
            // parse an integer
            input >> num;
            if (input.fail()) {
                
                input.clear();
                char temp;
                input >> temp;
            }
            else
                nums.push_back(num);

            // if eof bail out
            if (input.eof())
                break;

            
        }

        
        int posi_flag=0;
        for(unsigned vv:nums){
            if (vv<0){
                std::cout << "Error: the inputs must be all positive"<<std::endl;
                posi_flag=1;
            }
        }
        if (posi_flag==1){
            continue;
        }
        
        std::vector<int> s_path;
        if (command1=="V "){
            is_E=0;
            gg.num_ver=0;
            //is_s=0;
            gg.edge_vec.clear();
            if(nums[0]<2){
                std::cout<<"Error: the number of vertices should be at least 2"<<std::endl;
                continue;
            }
            gg.num_ver=nums[0];
        }
        if(gg.num_ver==0){
            std::cout<<"Error: the number of vertices haven't been defined"<<std::endl;
            continue;
        }
        if (command1=="E "){
            
            if (is_E==1){
                //std::cout<<"Error: the edges have been defined"<<std::endl;
                continue;
            }
            is_E=1;
            gg.edge_vec=nums;
            
        }
        int err_flag=0;
        for (unsigned each_num:gg.edge_vec){
            if(each_num>gg.num_ver){
                err_flag=1;
                break;
            }
        }
        if (err_flag==1){
            std::cout<<"Error: the edges shouln't contain vertices not exist"<<std::endl;
            gg.edge_vec.clear();
            //is_E=0;
            continue;
        }
        if (!gg.edge_vec.empty()){
            v_cover V_cover;

            for (int k=1;k<gg.edge_vec.size()+1;k++){
                V_cover=reduction(gg.edge_vec,gg.num_ver,k);
                if (V_cover.is_cover){
                    std::sort(V_cover.vers.begin(),V_cover.vers.end());
                    
                    for (int num:V_cover.vers){
                        std::cout<<num<<" ";
                    }
                    std::cout<<std::endl;
                    break;
                }
            }
        }
        if (gg.edge_vec.empty()&&is_E==1){
            std::cout<<std::endl;
        }
        
        
    }
}
