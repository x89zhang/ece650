#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <regex>
#include <random>
#include <fstream>
#include <string>
#include <unistd.h>

double tolerance=1e-6;
int attem_limit=25;

bool between_points(int x1, int y1, int x2, int y2, int xa, int ya){
    double doux1;
    double doux2;
    double douxa;
    double douy1;
    double douy2;
    double douya;
    if (xa==x1 && ya==y1){
        return false;
    }
    if (xa==x2 && ya==y2){
        return false;
    } 
    if (x1==x2){
        if (xa==x1){
            if ((ya-y1)*(ya-y2)<0){
                return true;
            }
        }
    } else{
        double grad=(douy2-douy1)/(doux2-doux1);
        double grad_test;
        if (xa==x1){
            grad_test=(douya-douy2)/(douxa-doux2);
        } else{
            grad_test=(douya-y1)/(douxa-doux1);
        }
        if (std::fabs(grad-grad_test)<=tolerance){
            if (y1==y2){
                if((xa-x1)*(xa-x2)<0){
                    return true;
                }
            } else{
                if ((ya-y1)*(ya-y2)<0){
                    return true;
                }
            }
        }
    }
    return false;
}
bool is_overlapping(int x1, int y1, int x2, int y2, int xa, int ya, int xb, int yb){
    double doux1=x1;
    double doux2=x2;
    double douxa=xa;
    double douxb=xb;
    double douy1=y1;
    double douy2=y2;
    double douya=ya;
    double douyb=yb;
    if(x1!=x2 && xa!=xb){
        double k1=(douy2-douy1)/(doux2-doux1);
        double k2=(douyb-douya)/(douxb-douxa);
    //difference in gradient means not overlapping
        if (std::fabs(k1-k2)>tolerance){
            return false;
        } else{
            //same gradient, check if (xa,ya) or (xb,yb) are on the segment of (x1,y1) and (x2,y2)
            if (between_points(x1,y1,x2,y2,xa,ya)||between_points(x1,y1,x2,y2,xb,yb)||between_points(xa,ya,xb,yb,x1,y1)||between_points(xa,ya,xb,yb,x2,y2)){
                return true;
            }

        }

    } else{
        if (x1==x2 && xa==xb && x2==xa){
            if(between_points(x1,y1,x2,y2,xa,ya)||between_points(x1,y1,x2,y2,xb,yb)||between_points(xa,ya,xb,yb,x1,y1)||between_points(xa,ya,xb,yb,x2,y2)){
                return true;
            }
        }
    }
    if (x1==xa && y1==ya && x2==xb && y2==yb){
        return true;
    }
    if (x1==xb && y1==yb && x2==xa && y2==ya){
        return true;
    }
    


    return false;
}
bool same_line(int x1,int y1,int x2,int y2,int xa,int ya){
    double doux1=x1;
    double doux2=x2;
    double douxa=xa;
    double douy1=y1;
    double douy2=y2;
    double douya=ya;
    if (x1==x2){
        if(xa==x1){
            return true;
        }
    } else{
        double grad=(y2-y1)/(x2-x1);
        double grad_test;
        if (xa==x1){
            grad_test=(douya-douy2)/(douxa-doux2);
        } else{
            grad_test=(douya-douy1)/(douxa-doux1);
        }
        if (std::fabs(grad-grad_test)<=tolerance){
            return true;
        }
    }
    return false;
}


    int number_streets(int num_st2){
        std::ifstream urandom("/dev/urandom");
        if (urandom.fail()) {
            std::cerr << "Error: cannot open /dev/urandom\n";
            return 1;
        }
        unsigned int range_int;
        urandom.read((char *)&range_int, sizeof(int));
        int st_num=2+(range_int%(num_st2-1));
        urandom.close();
        return st_num;
    }
    int number_segments(int num_segment2){
        std::ifstream urandom("/dev/urandom");
        if (urandom.fail()) {
            std::cerr << "Error: cannot open /dev/urandom\n";
            return 1;
        }
        unsigned int range_int;
        urandom.read((char *)&range_int, sizeof(int));
        int seg_num=1+(range_int%num_segment2);
        urandom.close();
        return seg_num;
    }
    
    int num_wait_time(int wait_time2){
        std::ifstream urandom("/dev/urandom");
        if (urandom.fail()) {
            std::cerr << "Error: cannot open /dev/urandom\n";
            return 1;
        }
        unsigned int range_int;
        urandom.read((char *)&range_int, sizeof(int));
        int time_num=5+(range_int%(wait_time2-4));
        urandom.close();
        return time_num;
    } 
    int val_cord(int cord_limit2){
        std::ifstream urandom("/dev/urandom");
        if (urandom.fail()) {
            std::cerr << "Error: cannot open /dev/urandom\n";
            return 1;
        }
        //double range_double;
        
        int range_double;
        urandom.read((char *)&range_double, sizeof(int));
        int val=-cord_limit2+(std::abs(range_double)%(cord_limit2*2+1));
        urandom.close();
        return val;
    }
    std::vector<int> generate_street(int num_seg, int co_limit){
        std::vector<int> st;
        int err1=0;
        int err2=0;
        int err3=0;
        for (int i=0; i<num_seg+1;i++){
            int x;
            int y;
            x=val_cord(co_limit);
            y=val_cord(co_limit);
            int sz=st.size();
            if(sz>=2){
                if (x==st[sz-2]&&y==st[sz-1]){
                    i--;
                    err1++;
                    if (err1>=attem_limit){
                        std::cerr<<"Error: failed to generate valid input for 25 simultaneous attempts"<<std::endl;
                        exit(1);
                    }
                    continue;
                } else{
                    err1=0;
                }
            }
            if(sz>=4){
                if(is_overlapping(st[sz-4],st[sz-3],st[sz-2],st[sz-1],st[sz-2],st[sz-1],x,y)){
                    i--;
                    err2++;
                    if (err2>=attem_limit){
                        std::cerr<<"Error: failed to generate valid input for 25 simultaneous attempts"<<std::endl;
                        exit(1);
                    }
                    continue;
                } else{
                    err2=0;
                }
                /*
                if(same_line(st[sz-4],st[sz-3],st[sz-2],st[sz-1],x,y)){
                    i--;
                    err3++;
                    if(err3>=attem_limit){
                        std::cerr<<"Error: failed to generate valid input for 25 simultaneous attempts"<<std::endl;
                        exit(1);
                    }
                    continue;
                } else{
                    err3=0;
                }*/

            }
            
            st.push_back(x);
            st.push_back(y);
        }
        return st;
    }


int main(int argc, char** argv){
    int street_num=10;
    int segment_num=5;
    int waiting_time=5;
    int coord_range=20;
    int cmd;
    while((cmd=getopt(argc,argv,"s:n:l:c:"))!=-1){
        switch(cmd){
            case 's':
                if (atoi(optarg)<2){
                    std::cerr<<"Error: invalid number of streets"<<std::endl;
                    exit(1);
                }
                street_num=atoi(optarg);
                break;
            case 'n':
                if (atoi(optarg)<1){
                    std::cerr<<"Error: invalid number of segments"<<std::endl;
                    exit(1);
                }
                segment_num=atoi(optarg);
                break;
            case 'l':
                if (atoi(optarg)<5){
                    std::cerr<<"Error: invalid wait time"<<std::endl;
                    exit(1);
                }
                waiting_time=atoi(optarg);
                break;
            case 'c':
                if (atoi(optarg)<1){
                    std::cerr<<"Error: invalid coordinate range"<<std::endl;
                    exit(1);
                }
                coord_range=atoi(optarg);
                break;
            case '?':
                std::cerr<<"Error: invalid input arguments"<<std::endl;
                exit(1);
            default:
                break;
        }
    }
    while(1){ 
    int num_st=number_streets(street_num);
    std::vector<std::string> st_list;
    std::vector<std::vector<int>> st_cord_list;
    st_list.clear();
    st_cord_list.clear();
    int err4=0;
    for (int i=0; i<num_st; i++){
        //std::string st_name1=std::to_string(i)+" "+"Street";
        //std::cout<<st_name1;
        int num_seg=number_segments(segment_num);
        //st_list.push_back(st_name1);
        std::vector<int> st=generate_street(num_seg, coord_range);
        bool ind_overlapping=false;
        for (std::vector<int> item:st_cord_list){
            for (int j=0; j<item.size(); j+=2){
                for (int k=0; k<st.size(); k+=2){
                    if (is_overlapping(item[j],item[j+1],item[j+2],item[j+3],st[k],st[k+1],st[k+2],st[k+3])){
                        //std::cout<<item[j]<<" "<<item[j+1]<<" "<<item[j+2]<<" "<<item[j+3]<<" "<<st[k]<<" "<<st[k+1]<<" "<<st[k+2]<<" "<<st[k+3]<<std::endl;
                        ind_overlapping=true;
                        break;
                    }
                }
            }
            
        }
        if (ind_overlapping){
            i--;
            err4++;
            if (err4>=attem_limit){
                std::cerr<<"Error: failed to generate valid input for 25 simultaneous attempts"<<std::endl;
                exit(1);
            }
            continue;
        } else{
            err4=0;
        }
        st_cord_list.push_back(st);
        std::string st_name1=std::to_string(i)+" "+"Street";
        st_list.push_back(st_name1);
/*
        std::string st_command="add \"" + st_name1 + "\"";
        for (int i=0; i<st.size(); i++){
            if (i%2==0){
                st_command=st_command+" "+"("+std::to_string(st[i]);
            } else{
                st_command=st_command+","+std::to_string(st[i])+")";
            }
        }
        std::cout<<st_command<<std::endl;
        */
        
    }
    for (int i=0; i<num_st; i++){
        std::string st_command="add \"" + st_list[i] + "\"";
        std::vector<int> temp_st=st_cord_list[i];
        for (int j=0; j<temp_st.size(); j++){
            if (j%2==0){
                st_command=st_command+" "+"("+std::to_string(temp_st[j]);
            } else{
                st_command=st_command+","+std::to_string(temp_st[j])+")";
            }
        }
        std::cout<<st_command<<std::endl;

    }
    std::cout<<"gg"<<std::endl;
    //issue rm commands to clear out existing streets
    for (std::string st_name2:st_list){
        std::string rm_command="rm \""+st_name2+"\"";
        std::cout<<rm_command<<std::endl;
    }
    int wait_time=num_wait_time(waiting_time);
    //std::cout<<wait_time<<std::endl;
    sleep(wait_time);
    
    }
}