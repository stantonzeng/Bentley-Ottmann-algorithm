#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <iostream>

using namespace std;

struct Seg{
    double startX;
    double startY;
    double endX;
    double endY;
    double slope;
    int label; //label of the line segment (a, b, c, or d... but this time we use integers)

    Seg(){

    }

    Seg(double sX, double sY, double eX, double eY, int l){
        this->startX = sX;
        this->startY = sY;
        this->endX = eX;
        this->endY = eY;
        this->label = l;
        this->slope = (eY-sY)/(eX-sX);
    }
};

struct Event{
    double posX;
    double posY;
    int type; //0 = start point, 1 = end point, 2 = intersection, -1 = Invalid
    int parentLabel1;
    int parentLabel2;

    Event(){}
    Event(double x, double y, int t, int l){ //Normal Line Segment
        this->posX = x;
        this->posY = y;
        this->type = t;
        this->parentLabel1 = l;
    }
    Event(double x, double y, int t, int l1, int l2){ //Intersection
        this->posX = x;
        this->posY = y;
        this->type = t;
        this->parentLabel1 = l1;
        this->parentLabel2 = l2;
    }
    friend bool operator< (const Event &left, const Event &right);
};

class Node{
    public:
        Seg val;
        Node *left;
        Node *right;
        int height;
        int balanceFactor;

        Node(){

        }
        Node(Seg val){
            this->val = val;
            this->left = NULL;
            this->right = NULL;
            this->height = 1;
        };
        
};
#endif