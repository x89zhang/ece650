#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <cctype>
#include <memory>
#include <pthread.h>
#include <time.h>
#include <iomanip>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

//augment from assignment 4
//commented code is we used to cout the running time
using namespace std;

struct ThreadData {
    vector<pair<int, int>> edgeList;
    int numVertices;
};

struct ThreadResult {
    vector<int> vc;
    double elapsed;
};

vector<int> edge_api(vector<pair<int, int>> edge_){
    vector<int> edge;
    for (pair<int,int> item:edge_){
        edge.push_back(item.first);
        edge.push_back(item.second);
    }
    return edge;
}

int find_Max(vector<int> vec){
    int max_ind=0;
    for (int i=0;i<int(vec.size());i++){
        if (vec[i]>vec[max_ind]){
            max_ind=i;
        }
    }
    return max_ind;
}

vector<int> Ver_count(vector<int> edge_vec,int V){
    vector<int> res(V+1,0);
    for (size_t i=0;i<edge_vec.size();i+=2){
        if (edge_vec[i]==-1||edge_vec[i+1]==-1){
            continue;
        }
        res[edge_vec[i]]=res[edge_vec[i]]+1;
        res[edge_vec[i+1]]=res[edge_vec[i+1]]+1;
    }
    return res;
}

int check_edge(const vector<pair<int, int>>& graph, int vnum) {
    for (const auto& edge : graph) {
        int u = edge.first;
        int v = edge.second;

        if (u < 1 || u > vnum || v < 1 || v > vnum) {
            return 1;  
        }

        if (u == v) {
            return 2;  
        }
    }
    return 0;  
}

vector<int> find_lvc_cnf_reduct(vector<pair<int, int>> edgeList, int num_vertices) {
    bool found = false; 
    int k = 1;

    unique_ptr<Minisat::Solver> solver(new Minisat::Solver());

    while(found == false) {
        vector<vector<Minisat::Lit>> V(k, vector<Minisat::Lit>(num_vertices + 1)); 
        for (int i = 0; i < k; i++) {
            for (int j = 1; j <= num_vertices; j++) {
                V[i][j] = Minisat::mkLit(solver->newVar());
            }
        }

        for (int i = 0; i < k; i++) {
            Minisat::vec<Minisat::Lit> clause1;
            for (int j = 1; j <= num_vertices; j++) {
                clause1.push(V[i][j]);
            }
            solver->addClause(clause1);
        }

        if (k > 1) {
            for (int m = 1; m <= num_vertices; m++) {
                for (int p = 0; p < k; p++) {
                    for (int q = p + 1; q < k; q++) {
                        solver->addClause(~V[p][m], ~V[q][m]);
                    }
                }
            }
        }

        for (int m = 0; m < k; m++) {
            for (int p = 1; p <= num_vertices; p++) {
                for (int q = p + 1; q <= num_vertices; q++) {
                    solver->addClause(~V[m][p], ~V[m][q]);
                }
            }
        }

        for (int i = 0; i < int(edgeList.size()); i++) {
            Minisat::vec<Minisat::Lit> clause4;
            for (int j = 0; j < k; j++) {
                int cur_first = edgeList[i].first;
                int cur_second = edgeList[i].second;
                clause4.push(V[j][cur_first]);
                clause4.push(V[j][cur_second]);
            }
            solver->addClause(clause4);
        }

        bool res = solver->solve();

        if (res) {
            found = true;
            vector<int> vertex_cover;

            for (int i = 0; i < k; i++) {
                for (int j = 1; j <= num_vertices; j++) {
                    if (Minisat::toInt(solver->modelValue(V[i][j])) == 0) {
                        vertex_cover.push_back(j); 
                        break;
                    }
                }
            }

            sort(vertex_cover.begin(), vertex_cover.end());

            return vertex_cover;
        }

        k++;
        solver.reset(new Minisat::Solver());
    }
}

void* thread_find_lvc_cnf_reduct(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    vector<pair<int, int>>& edgeList = data->edgeList;
    int num_vertices = data->numVertices;

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    vector<int> vc_cnf = find_lvc_cnf_reduct(edgeList, num_vertices);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    ThreadResult* result = new ThreadResult();
    result->vc = vc_cnf;
    result->elapsed = elapsed;

    return static_cast<void*>(result);
}

vector<int> find_lvc_vc1(vector<pair<int, int>> edge_vec1, int V){
    vector<int> res;
    vector<int> edge_vec=edge_api(edge_vec1);
    while(1){
        vector<int> ver_c=Ver_count(edge_vec,V);
        int max_ind=find_Max(ver_c);
        if (max_ind==0){
            break;
        }
        res.push_back(max_ind);
        for(size_t i=0;i<edge_vec.size();i+=2){
            if(edge_vec[i]==max_ind||edge_vec[i+1]==max_ind){
                edge_vec[i]=-1;
                edge_vec[i+1]=-1;
            }
        }
    }
    sort(res.begin(), res.end());
    return res;
}

void* thread_find_lvc_vc1(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    vector<pair<int, int>>& edgeList = data->edgeList;
    int num_vertices = data->numVertices;

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    vector<int> vc_vc1 = find_lvc_vc1(edgeList, num_vertices);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    ThreadResult* result = new ThreadResult();
    result->vc = vc_vc1;
    result->elapsed = elapsed;

    return static_cast<void*>(result);
}

vector<int> find_lvc_vc2(vector<pair<int, int>> edge_vec1, int V){
    vector<int> edge_vec=edge_api(edge_vec1);
    vector<int> res;
    for(size_t i=0;i<edge_vec.size();i+=2){
        if (edge_vec[i]!=0 && edge_vec[i+1]!=0){
            int u=edge_vec[i];
            int v=edge_vec[i+1];
            res.push_back(u);
            res.push_back(v);
            for (size_t j=0;j<edge_vec.size();j+=2){
                if (edge_vec[j]==u||edge_vec[j+1]==u||edge_vec[j]==v||edge_vec[j+1]==v){
                    edge_vec[j]=0;
                    edge_vec[j]=0;
                }
            }
        }
    }
    sort(res.begin(), res.end());
    return res;
}

void* thread_find_lvc_vc2(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    vector<pair<int, int>>& edgeList = data->edgeList;
    int num_vertices = data->numVertices;

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);
    vector<int> vc_vc2 = find_lvc_vc2(edgeList, num_vertices);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    ThreadResult* result = new ThreadResult();
    result->vc = vc_vc2;
    result->elapsed = elapsed;

    return static_cast<void*>(result);
}

int main() {
    int v;
    string line;
    bool v_check = false;
    bool timeout = false;

    while (getline(cin, line)) {

        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        char inputType;

        ss >> inputType;

        if (inputType == 'V') {
            int V;
            ss >> V;  
            if (V > 1) {
                v = V;
                v_check = true;
            } else {
                cout << "Error: Number of vertices must be greater than 1" << endl;
            }
        }

        if (inputType == 'E') {
            if (v_check == true){
                string edges;
                ss >> edges;  

                vector<pair<int, int>> edgeList;
                int length = edges.length();
                
                for (int i = 0; i < length; ++i) {
                    while (i < length && !isdigit(edges[i])) {
                        ++i;
                    }

                    if (i >= length) {
                        break;
                    }

                    int start = i;
                    while (i < length && isdigit(edges[i])) {
                        ++i;
                    }

                    int firstNum = stoi(edges.substr(start, i - start));

                    while (i < length && !isdigit(edges[i])) {
                        ++i;
                    }


                    if (i >= length) {
                        break;
                    }

                    start = i;
                    while (i < length && isdigit(edges[i])) {
                        ++i;
                    }

                    int secondNum = stoi(edges.substr(start, i - start));

                    edgeList.emplace_back(firstNum, secondNum);
                }

                int edgeCheck;
                edgeCheck = check_edge(edgeList, v);
                if (edgeCheck == 0) {

                    pthread_t thread_cnf, thread_cv1, thread_cv2;

                    ThreadData data;
                    data.edgeList = edgeList;
                    data.numVertices = v;
                  
                    pthread_create(&thread_cv1, nullptr, thread_find_lvc_vc1, &data);
                    pthread_create(&thread_cv2, nullptr, thread_find_lvc_vc2, &data);

                    struct timespec start, now;
                    clock_gettime(CLOCK_MONOTONIC, &start);

                    pthread_create(&thread_cnf, nullptr, thread_find_lvc_cnf_reduct, &data);

                    void* result_cnf_ptr;

                    while (true) {
                        clock_gettime(CLOCK_MONOTONIC, &now);
                        double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1000000000.0;

                        if (elapsed > 120.0) {  
                            pthread_cancel(thread_cnf);
                            cout << "CNFSAT Thread timeout exceeded. Terminating thread." << endl;
                            timeout = true;
                            break;
                        }

                        if (pthread_tryjoin_np(thread_cnf, &result_cnf_ptr) == 0) {
                          
                            break;
                        }

                        usleep(100000);
                    }
                    
                    void* result_cv1_ptr;
                    void* result_cv2_ptr;

                    pthread_join(thread_cv1, &result_cv1_ptr);
                    pthread_join(thread_cv2, &result_cv2_ptr);

                    ThreadResult* result_cnf = static_cast<ThreadResult*>(result_cnf_ptr);
                    ThreadResult* result_cv1 = static_cast<ThreadResult*>(result_cv1_ptr);
                    ThreadResult* result_cv2 = static_cast<ThreadResult*>(result_cv2_ptr);

                    vector<int> vc_cnf = result_cnf->vc;
                    double elapsed_cnf = result_cnf->elapsed;
                    vector<int> vc_cv1 = result_cv1->vc;
                    double elapsed_cv1 = result_cv1->elapsed;
                    vector<int> vc_cv2 = result_cv2->vc;
                    double elapsed_cv2 = result_cv2->elapsed;
                    
                    elapsed_cnf = elapsed_cnf * 1000000;
                    elapsed_cv1 = elapsed_cv1 * 1000000;
                    elapsed_cv2 = elapsed_cv2 * 1000000;

                    if (!timeout) {
                        cout<<"CNF-SAT-VC: ";
                        for (int i = 0; i < int(vc_cnf.size()); i++) {
                            cout << vc_cnf[i] << ' ';
                        }
                        cout << endl;
                    }

                    cout<<"APPROX-VC-1: ";
                    for (int i = 0; i < int(vc_cv1.size()); i++) {
                        cout << vc_cv1[i] << ' ';
                    }
                    cout << endl;

                    cout<<"APPROX-VC-2: ";
                    for (int i = 0; i < int(vc_cv2.size()); i++) {
                        cout << vc_cv2[i] << ' ';
                    }
                    cout << endl;
                    
                    /*\if (!timeout) {
                        cout << "CNF elapsed time: " << fixed << setprecision(9) << elapsed_cnf << " micro seconds" << endl;
                    }
                    cout << "VC1 elapsed time: " << fixed << setprecision(9) << elapsed_cv1 << " micro seconds" << endl;
                    cout << "VC2 elapsed time: " << fixed << setprecision(9) << elapsed_cv2 << " micro seconds" << endl;*/

                } else if (edgeCheck == 1) {
                    cout << "Error: Vertex out of range" << endl;
                } else if (edgeCheck == 2) {
                    cout << "Error: The vertices of an edge can't be the same" << endl;
                }
            } else if (v_check == false) {
                cout << "Error: V have to be given before E" << endl;
            }
        } else if (inputType == 'E') {
        }
    }
    return 0;
}