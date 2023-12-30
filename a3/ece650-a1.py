#!/usr/bin/env python3
import sys
import re
import math
import sys

#class street takes the street name and coordinates as input
class Street:
    def __init__(self,line):
        command=line.strip().split('" ')
        self.street_name=command[0]
        self.coordinates=self.processing_coordinates(command[1])
        self.segments=self.create_segments(command[1])
    def processing_coordinates(self,cmd_coordinates):
       coordinate=[]
       string_coordinates=cmd_coordinates.strip().split(' ')
       for element in string_coordinates:
           stri=element.strip('()').split(',')
           x,y=map(float,stri)
           coordinate.append((x,y))
       return coordinate
    #coordinate is a list contains the coordinate points in tuple specified in a street command
    def create_segments(self,cmd_coordinates):
        segment=[]
        coordinate=self.processing_coordinates(cmd_coordinates)
        
        for i in range(len(coordinate) - 1):
            seg=(coordinate[i],coordinate[i+1])
            segment.append(seg)
        return segment
    #This will return a list contains segments in tuple then in tuple for a street
    def on_which_segment(self,point):
        Num=[]
        for ind, seg in enumerate(self.segments):
            start_point=seg[0]
            end_point=seg[1]
            grad=(start_point[1]-end_point[1])/(start_point[0]-end_point[0]) if start_point[0] !=end_point[0] else float('inf')
            if point==start_point:
                grad_test=(end_point[1]-point[1])/(end_point[0]-point[0]) if end_point[0] !=point[0] else float('inf')
            else:
                grad_test=(start_point[1]-point[1])/(start_point[0]-point[0]) if start_point[0] !=point[0] else float('inf')
            if math.isclose(grad,grad_test):
                if (point[0]-start_point[0])*(point[0]-end_point[0])<=0 and (point[1]-start_point[1])*(point[1]-end_point[1])<=0:
                    Num.append(ind)
        return Num
    def same_segment(self,p1,p2):
        tr1=self.on_which_segment(p1)
        tr2=self.on_which_segment(p2)
        for num1 in tr1:
            for num2 in tr2:
                if num1==num2:
                    return True
        return False
                
            



#This subroutine is based on the ece650 python examples, change to the version to detect intersection of segments 
def intersect(l1,l2):
    (x1,y1)=l1[0]
    (x2,y2)=l1[1]
    (x3,y3)=l2[0]
    (x4,y4)=l2[1]
    xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4))
    xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4))
    if xden==0:
        return None
    xcoor =  xnum / xden
    ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4)
    yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4)
    if yden==0:
        return None
    ycoor = ynum / yden
    if (xcoor-x1)*(xcoor-x2)<=0 and (ycoor-y1)*(ycoor-y2)<=0:
        if (xcoor-x3)*(xcoor-x4)<=0 and (ycoor-y3)*(ycoor-y4)<=0:
       
            return (xcoor,ycoor)
    return None
#intersection is a tuple representing the coordinate


def add_street(line,st):
    count=0
    command=line.split('" ')
    for st_class in st:
        if st_class.street_name==command[0]:
            count=1
    if count==1:
        print('Error: this street has already been added, you cannot add it again')
        return st
    street=Street(line)
    st.append(street)
    return st
#st is the list contain class streets

def modify_street(line,st):
    count=0
    command=line.split('" ')
    for ind,st_class in enumerate(st): 
        if st_class.street_name==command[0]:
            st[ind]=Street(line)
            count=1
    if count==0:
        print("Error: couldn't find this street in the graph, please input an existing street")
    return st     

def generate_graph(st):
    ver_set1=set()
    ver_set2=set()
    #intersection
    for st_class1 in st:
        for st_class2 in st:
            for ind,seg1 in enumerate(st_class1.segments):
                for seg2 in st_class2.segments:
                    if st_class1==st_class2:
                        continue

                    #seg1 and seg2 are segment and .segments are all segments in a street
                    if seg1==seg2:
                        continue
                    insec=intersect(seg1,seg2)
                    if insec==None:
                        continue
                    ver_set1.add(insec)
                    if st_class1!=st_class2:
                        ver_set2.add(seg1[0])
                        ver_set2.add(seg1[1])
                        ver_set2.add(seg2[0])
                        ver_set2.add(seg2[1])             

    
    ver_set=ver_set1 | ver_set2
    #ver_set1 is set for intersections tuples and ver_set2 is set for start and end points tuples
    ver=list(ver_set)
    
    # ver is a list contains the coordinates of the desired points in tuples
    ver_set1_list=list(ver_set1)
    
    
    edge_set=set()
    for inters_ in ver_set1_list: 
        for vers_2 in ver: 
            if inters_==vers_2:
                continue
            for st_class3 in st:
                tr1=st_class3.on_which_segment(inters_)
                tr2=st_class3.on_which_segment(vers_2)
                
                if len(tr1)!=0 and len(tr2)!=0:
                    traverse=False
                    #inters_ and vers_2 on the same street
                    #inters_ and vers_2 are on the same segment
                    same_seg=st_class3.same_segment(inters_,vers_2)
                    if same_seg==False:
                        continue
                    if same_seg==True:
                        for vers_3 in ver:
                            if st_class3.same_segment(inters_,vers_3)==True and st_class3.same_segment(vers_2,vers_3)==True:
                                
                                if (vers_3[0]-vers_2[0])*(vers_3[0]-inters_[0])<0 or (vers_3[1]-vers_2[1])*(vers_3[1]-inters_[1])<0:
                                    
                                    
                                    traverse=True
                                    break
                    if traverse==False:
                        #print((inters_,vers_2))
                        #iners_ and vers_2 build up an edge
                        edge_set.add((inters_,vers_2))
    edge=list(edge_set)

    #remove repeting edges
    for k1 in edge:
        for k2 in edge:
            if k1[0]==k2[1]:
                if k1[1]==k2[0]:
                    edge.remove(k2)
    
    return ver, edge





      


def remove_street(line,st):
    count=0
    for st_class in st: 
        
        if st_class.street_name==line:
            st.remove(st_class)
            count=1
    if count==0:
        print('Error: you cannot remove a street does not exist')
    return st  

    
def main():
    
    st=[]
    cmd_count=0
    while True:
        line = sys.stdin.readline()
        
        if line == "":
            break
        #print("read a line:", line)
        if line.startswith('add ')==False and line.startswith('mod ')==False and line.startswith('rm ')==False and line.startswith('gg')==False:
            print('Error: only "add", "mod", "rm" and "gg" command is permitted')
            continue
        try:
            
            command=line.split('"')
        except:
            print('Error: the input should be a command in string')
            continue
        
        
        if line.startswith('add ') or line.startswith('mod ') or line.startswith('rm ') or command[0]=='gg'+'\n':
                
            
                if line.startswith('add '):

                        if len(command)!=3:
                            print('Error: add command should be specified as: add "Street Name" coordinates, please check you specification')
                            continue
                        format1=r"\(\s*[-+]?\d+(\.\d+)?\s*,\s*[-+]?\d+(\.\d+)?\s*\)"
                        com=command[2]
                        com1=com.strip()
                        
                        coord=com1.split(' ')
                        
                        if len(coord)<2:
                            print('Error: at least two coordinate points are needed')
                            continue
                        iod=0
                        for co in coord:
                            if_match=re.fullmatch(format1,co)
                            if not if_match:
                                
                                iod=1
                                break
                        if iod==1:
                            print('Error: some coordinate points are invalid, please input again')
                            continue
                        st_line=command[1]+'"'+command[2]
                        st=add_street(st_line,st)
                        #print('Adding complete...')
                        
                
                if line.startswith('rm '):
                        if len(command[2])!=1:
                            print('Error: remove command should be specified as: rm "Street Name", please check you specification')
                            continue
                        st_line=command[1]  
                        st=remove_street(st_line,st)
                        #print('Removing complete...')

                if line.startswith('mod '):
                        if len(command)!=3:
                            print('Error: mod command should be specified as: add "Street Name" coordinates, please check you specification')
                            continue
                        format1=r"\(\s*[-+]?\d+(\.\d+)?\s*,\s*[-+]?\d+(\.\d+)?\s*\)"
                        com=command[2]
                        com1=com.strip()
                        
                        coord=com1.split(' ')
                        
                        if len(coord)<2:
                            print('Error: at least two coordinate points are needed')
                            continue
                        iod=0
                        for co in coord:
                            if_match=re.fullmatch(format1,co)
                            if not if_match:
                                
                                iod=1
                                break
                        if iod==1:
                            print('Error: some coordinate points are invalid, please input again')
                            continue
                        st_line=command[1]+'"'+command[2]
                        st=modify_street(st_line,st)
                        #print('Modifying complete...')
                
                if command[0]=='gg\n':
                        if line!='gg\n':
                            print('Error: wrong usage for command gg')
                            continue

                        #print('Generating graph...')
                        ver,edge=generate_graph(st)
                        print('V '+str(len(ver)))
                        sys.stdout.flush()
                        #for index_ver,vers in enumerate(ver):
                            
                            #print('\t'+str(index_ver+1)+': '+str(vers))
                        #print('}')
                        
                        print('E {',end='')
                        for ind_ed,ed in enumerate(edge):
                            for index_ver1,vers1 in enumerate(ver):
                                
                                if ed[0]==vers1:
                                    left=index_ver1+1
                                if ed[1]==vers1:
                                    right=index_ver1+1
                            
                                    
                                    
                            if ind_ed==len(edge)-1:
                                print('<'+str(left)+','+str(right)+'>',end='')
                            else:
                                print('<'+str(left)+','+str(right)+'>,',end='')
                        print('}')
                        sys.stdout.flush()

                        #print('Generating complete...')
                    
            
    #print("Finished reading input")
    # return exit code 0 on successful termination
    sys.exit(0)


if __name__ == "__main__":
    main()
