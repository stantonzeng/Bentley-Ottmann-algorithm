#include<iostream>
#include<vector>
#include <queue>
#include <set>
#include <map>
#include <cmath>
#include "node.hpp"
// #include "avl.hpp"

using namespace std;

/*
Note for when I wake up:

I think the reason why we needed an actual BST instead of a priority queue is because when we want to delete an element, this 
is a simple log(n) time.

Unfortunately, since we are using a priority queue, we cannot really delete an element that is not at the top.
Our solution will be to create another priority queue, but just input all the values except for the value we want to delete. We can 
kill two birds with one stone by also doing neighbor comparisons as we are poping out all of the elements into the new pq.

Similarly, since we are doing comparisons, two lines only have to compare with each other once (fact check me on this one). This only changes
at an intersection, where only two lines become effected, or when a new line enter/old line expires. Again, this won't do much, 
but in larger values this becomes a lot more fucked. 

*/


double currPos = 0;
set<int> removeLabels;

float epsilon = 0.000001f;

class Compare{
public:
    //Compare Operator for our priority queue
    bool operator() (Seg &a, Seg &b){ 
        if(fabs(((a.slope*(currPos - a.startX))+a.startY) - ((b.slope*(currPos - b.startX))+b.startY)) < epsilon){
            double tempCurrPos = currPos + 1;
            return (a.slope*(tempCurrPos - a.startX))+a.startY < (b.slope*(tempCurrPos - b.startX))+b.startY;
        }
        return (a.slope*(currPos - a.startX))+a.startY < (b.slope*(currPos - b.startX))+b.startY;
    }
};

set<Event> events; //Holds all of the events at which the line will reach them
set<pair<int, int>> st; //Holds the lines that we have already checked
map<int, Seg> mp; //Holds ALL of the segments (unchecked or checked) using their label as their key.
priority_queue<Seg, vector<Seg>, Compare> pq; //Holds the order at which there are neighboring segments and when we check their intersection
vector<Event> answ; //Holds all of the intersections we find

//Comparator for the set, events.
bool operator< (const Event &a, const Event &b){
    if(a.posX == b.posX) return a.posY < b.posY;
    return a.posX < b.posX;
}

void insertSegment(Seg a){
    events.insert(Event(a.startX, a.startY, 0, a.label));
    events.insert(Event(a.endX, a.endY, 1, a.label));
    mp[a.label] = a;
}

Event findIntersection(Seg a, Seg b){
    double slopeA = (a.endY-a.startY)/(a.endX-a.startX);
    double slopeB = (b.endY-b.startY)/(b.endX-b.startX);

    if(slopeA == slopeB){
        return Event(-1, -1, -1, -1);
    }

    //System of Equations in matrix form
    double eA = (-1*slopeA*a.startX) + a.startY;
    double eB = (-1*slopeB*b.startX) + b.startY;

    vector<vector<double>> mat {{-1*slopeA, 1}, {-1*slopeB, 1}};

    double det = 1/(mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0]);

    vector<vector<double>> invMat {{mat[1][1]*det, -1*mat[0][1]*det}, {-1*mat[1][0]*det, mat[0][0]*det}};

    Event temp(invMat[0][0]*eA + invMat[0][1]*eB, invMat[1][0]*eA + invMat[1][1]*eB, 2, a.label, b.label);
    return temp;
}

bool checkIntersection(Seg a, Seg b, double xVal, double yVal){
    if((min(a.startX, a.endX) > xVal || xVal > max(a.startX, a.endX)) || (min(b.startX, b.endX) > xVal || xVal > max(b.startX, b.endX))){
        return false;
    }
    else if((min(a.startY, a.endY) > yVal || yVal > max(a.startY, a.endY)) || (min(b.startY, b.endY) > yVal || yVal > max(b.startY, b.endY))){
        return false;
    }
    return true;
}

void printPQ(priority_queue<Seg, vector<Seg>, Compare> p){
    cout << "Priority Queue: ";
    while(!p.empty()){
        cout << p.top().label << " ";
        p.pop();
    }
    cout << endl;
}

void checkNeighbors(){
    printPQ(pq);
    priority_queue<Seg, vector<Seg>, Compare> pqCopy;
    Seg com;

    //Get the first segment we need to look at
    while(!pq.empty()){
        if(removeLabels.count(pq.top().label) == 0){
            com = pq.top();
            pqCopy.push(com);
            pq.pop();
            break;
        }
        removeLabels.erase(pq.top().label);
        pq.pop();
    }

    while(!pq.empty()){
        pair <int, int> p = make_pair(min(com.label, pq.top().label), max(com.label, pq.top().label));

        cout << "Com: " << com.label << ", top: " << pq.top().label << endl;

        cout << "Set: " << p.first << " " << p.second << " " << st.count(p) << endl;
        cout << "RemoveLabel Counter: " << pq.top().label << " " << removeLabels.count(pq.top().label) << endl;

        if(removeLabels.count(pq.top().label) != 0){
            removeLabels.erase(pq.top().label);
        }
        //If we have not seen this exact pair of neighbors before and we are not trying to remove the segment
        else if(st.count(p) == 0){ 
            cout << "Checking Neighbors: " << com.label << " and " << pq.top().label << endl;

            Event e = findIntersection(com, pq.top()); //Get Intersection
        
            //If the intersection is valid
            if(checkIntersection(com, pq.top(), e.posX, e.posY)){ 
                
                //Then insert it into the events set that we will itterate over
                events.insert(Event(e.posX, e.posY, 2, com.label, pq.top().label));
        
            }
            
            //insert the pair of neighbors so we don't have to check them again
            st.insert(p);

            pqCopy.push(pq.top());
            com = pq.top();
        }
        else{
            pqCopy.push(pq.top());
            com = pq.top();
        }
        //Reassign com before removing the top
        pq.pop();
    }
    pq = pqCopy;
}



int main(){
    
    Seg p1(2,5,4,1,1);
    Seg p2(3,2,5,4,2);
    Seg p3(1,4,6,2,3);
    Seg p4(8,3,7,5,4);

    insertSegment(p1);
    insertSegment(p2);
    insertSegment(p3);
    insertSegment(p4);
    
    for(auto it = events.begin(); it != events.end(); it++){

        currPos = it->posX; //Set the current Position of the line we are at right now
        cout << endl << "At position: " << it->posX << "," << it->posY << " " << endl;

        if(it->type == 2){//Intersection
            cout << "Type: Intersection of " << it->parentLabel1 << " and " << it->parentLabel2 << endl;
            //First, we need to "remove" the two segments 
            removeLabels.insert(it->parentLabel1);
            removeLabels.insert(it->parentLabel2);

            //Then we need to re-add them in their new order, essentially flipping them.
            pq.push(mp[it->parentLabel1]);
            pq.push(mp[it->parentLabel2]);

            
            cout << "Elements in RemoveLabel: ";
            for(auto it2 = removeLabels.begin(); it2 != removeLabels.end(); it2++){
                cout << *it2 <<  " ";
            }
            cout << endl;

            //Lastly, we push back the event into our list of intersections
            answ.push_back(*it);
        }
        else if(it->type == 0){//Start Point
            cout << "Type: Start Point of " << it->parentLabel1 << endl;

            cout << "Elements in RemoveLabel: ";
            for(auto it2 = removeLabels.begin(); it2 != removeLabels.end(); it2++){
                cout << *it2 <<  " ";
            }
            cout << endl;
            pq.push(mp[it->parentLabel1]); //Pushing in the segment based on the event's parent label
        }
        else if(it->type == 1){//End Point
            cout << "Type: End Point of " << it->parentLabel1 << endl;

            cout << "Elements in RemoveLabel: ";
            for(auto it2 = removeLabels.begin(); it2 != removeLabels.end(); it2++){
                cout << *it2 <<  " ";
            }
            cout << endl;
            removeLabels.insert(it->parentLabel1); //Signaling to the set we want this segment removed
        }

        if(it->type != -1){
            checkNeighbors();
        }
    }

    for(int i = 0; i < answ.size(); i++){
        cout << "(" << answ[i].posX << "," << answ[i].posY << ") ";
    }
    cout << endl;

    
    return 0;
}