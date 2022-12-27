#include <bits/stdc++.h>
using namespace std;

class node{
public:
    int nonLeaf;
    int size;
    int *val;

    node(){}
    node(int m, int next) {
        this->size = m * 2;
        this->nonLeaf = -1;
        val = new int[(m * 2)];
        val[0] = next;
        for (int i = 1; i < (m * 2); ++i) {
            val[i] = -1;
        }
    }
};
