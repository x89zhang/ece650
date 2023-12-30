// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <regex>

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
            if (vv<=0){
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
        if (command1=="s "){
            if(gg.edge_vec.empty()){
                std::cout<<"Error: the edges haven't been defined"<<std::endl;
                continue;
            }
            if (nums[0]>gg.num_ver||nums[1]>gg.num_ver){
                std::cout<<"Error: the vertices don't exist"<<std::endl;
                continue;
            }
            s_path=gg.shortest_path(nums[0],nums[1]);
            

        }
        
        
        if (!s_path.empty()){
            if(s_path.front()==-1){
                std::cout<<"Error: the path doesn't exist"<<std::endl;
                continue;
            }
        }
        
        
        if (!s_path.empty()){
            for (int vet:s_path){
                if (vet==s_path.back()){
                    std::cout<<vet<<std::endl;
                    break;
                }
                std::cout<<vet<<'-';
            }
        }
        
        
    }
}


