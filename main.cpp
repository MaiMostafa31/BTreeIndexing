#include <bits/stdc++.h>
#include "node.h"

using namespace std;

pair<int, vector<int>> getLeaf(const char *filename, node &root, int id) {
    fstream Btree;
    Btree.open(filename, ios::in);
    vector<int> v;
    int target = -1, i;
    bool flag = false;
    for (i = 0; i < root.size and root.val[i] != -1; i += 2) {
        if (root.val[i] >= id) {
            flag = true;
            target = root.val[i + 1];
            Btree.seekg(target * sizeof(root), ios::beg);
            node n;
            Btree.read((char *) &n, sizeof(root));
            while (n.nonLeaf) {
                int j;
                for (j = 0; j < n.size and n.val[j] != -1; j += 2) {
                    if (n.val[j] >= id) {
                        target = n.val[j + 1];
                        Btree.seekg(target * sizeof(root), ios::beg);
                        Btree.read((char *) &n, sizeof(root));
                        break;
                    }
                }
            }
            break;
        }

    }
    if (!flag) {
        v.push_back(1);
        target = root.val[--i];
        v.push_back(target);
        node n;
        Btree.seekg(target * sizeof(root), ios::beg);
        Btree.read((char *) &n, sizeof(root));
        while (n.nonLeaf) {
            int j;
            for (j = 0; j < n.size and n.val[j] != -1; j += 2) {
                target = n.val[j + 1];
            }
            v.push_back(target);
            Btree.seekg(target * sizeof(root), ios::beg);
            Btree.read((char *) &n, sizeof(root));

        }
    }
    return {target, v};
}

void sortNode(int *arr, int indx) {
    vector<pair<int, int>> v;
    for (int i = 0; i <= indx; i += 2) {
        v.emplace_back(arr[i], arr[i + 1]);
    }
    sort(v.begin(), v.end());
    for (int i = 0, j = 0; i <= indx; i += 2, j++) {
        arr[i] = v[j].first;
        arr[i + 1] = v[j].second;
    }
}

void
splitRoot(node &root, node &node1, node &node2, int place1, int place2, int id = -1, int ref = -1, bool flag = false) {
    int i;
    root.nonLeaf = 1;
    if (!flag) {
        node1.nonLeaf = 0;
        node2.nonLeaf = 0;
    } else {
        node1.nonLeaf = 1;
        node2.nonLeaf = 1;
    }
    vector<pair<int, int>> v;
    for (int x = 0; x < root.size; x += 2) {
        v.emplace_back(root.val[x], root.val[x + 1]);
    }
    if (id != -1 && ref != -1)
        v.emplace_back(id, ref);
    sort(v.begin(), v.end());
    int j = 0;
    for (i = 0; i < (v.size() / 2); i++) {
        node1.val[j] = v[i].first;
        node1.val[j + 1] = v[i].second;
        j += 2;
    }
    int max1 = node1.val[j - 2];
    j = 0;
    while (i < v.size()) {
        node2.val[j] = v[i].first;
        node2.val[j + 1] = v[i].second;
        j += 2;
        i++;
    }
    int max2 = node2.val[j - 2];
    root.val[0] = max1;
    root.val[1] = place1;
    root.val[2] = max2;
    root.val[3] = place2;
    for (int k = 4; k < root.size; ++k) {
        root.val[k] = -1;
    }

}

bool splitLeaf(node &parent, node& tosplit, node &splitIn, int placeofsplitting) {
    splitIn.nonLeaf = 0;
    if (parent.val[parent.size - 1] != -1) {
        return false;
    }
    vector<pair<int, int>> v;
    for (int x = 0; x < tosplit.size; x += 2) {
        v.emplace_back(tosplit.val[x], tosplit.val[x + 1]);
    }

    int j = 0;
    for (j = tosplit.size / 2; j < tosplit.size; j += 2) {
        tosplit.val[j] = -1;
        tosplit.val[j + 1] = -1;
    }
    int max1 = tosplit.val[tosplit.size / 2 - 2];
    j = 0;
    for (int i = v.size() / 2; i < v.size(); i++) {
        splitIn.val[j] = v[i].first;
        splitIn.val[j + 1] = v[i].second;
        j += 2;
    }
    int max2 = splitIn.val[j - 2];
    for (int k = 0; k < parent.size; ++k) {
        if (parent.val[k] == -1) {
            int placeOfToSplit = parent.val[k - 1];
            parent.val[k - 2] = max2;
            parent.val[k - 1] = placeofsplitting;
            parent.val[k] = max1;
            parent.val[k + 1] = placeOfToSplit;
            break;
        }
    }
    return true;
}

void CreateIndexFileFile(const char *filename, int numberOfRecords, int m) {
    // write initial state of index file where all nodes are empty
    fstream Btree;
    Btree.open(filename, ios::in | ios::out | ios::binary | ios::app);
    for (int i = 0; i < numberOfRecords - 1; i++) {
        node write(m, i + 1);
        Btree.write((char *) &write, sizeof(write));

    }
    node write(m, -1);
    Btree.write((char *) &write, sizeof(write));
    Btree.close();
}

void DisplayIndexFileContent(const char *filename) {
    fstream Btree;
    Btree.open(filename, ios::in | ios::out | ios::binary | ios::app);
    Btree.seekg(0, ios::beg);
    node read;
    while (Btree.read((char *) &read, sizeof(read))) {
        cout << read.nonLeaf << " ";
        for (int i = 0; i < read.size; ++i) {
            cout << read.val[i] << " ";
        }
        cout << endl;
    }
    cout << endl;
    Btree.close();
}

int InsertNewRecordAtIndex(const char *filename, int RecordID, int Reference) {
    fstream Btree;
    Btree.open(filename, ios::in | ios::out | ios::binary);
    Btree.seekg(0, ios::beg);
    node header;
    Btree.read((char *) &header, sizeof(header));
    Btree.seekg(sizeof(header), ios::beg);
    node root;
    Btree.read((char *) &root, sizeof(root));
    //tree is empty-> no nodes
    if (header.val[0] == 1) {
        header.val[0]++;

        root.nonLeaf = 0;
        root.val[0] = RecordID;
        root.val[1] = Reference;
        Btree.seekp(0, ios::beg);
        Btree.write((char *) &header, sizeof(header));
        Btree.write((char *) &root, sizeof(root));
        Btree.close();
        return 0;


    } else {

        //tree is 1 level-> root only
        if (root.nonLeaf == 0) {
            int i;
            bool flag = false;
            for (i = 0; i < root.size; i += 2) {
                if (root.val[i] == -1) {
                    root.val[i] = RecordID;
                    root.val[i + 1] = Reference;
                    sortNode(root.val, i + 1);
                    Btree.seekp(sizeof(header), ios::beg);
                    Btree.write((char *) &root, sizeof(root));
                    Btree.close();
                    flag = true;
                    return 0;
                }
            }
            //tree is 1 level, root is full-> split
            if (!flag) {
                int place1 = header.val[0];
                Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                node node1;
                Btree.read((char *) &node1, sizeof(node1));
                int place2 = node1.val[0];
                Btree.seekg(node1.val[0] * sizeof(node1), ios::beg);
                node node2;
                Btree.read((char *) &node2, sizeof(node2));
                header.val[0] = node2.val[0];
                splitRoot(root, node1, node2, place1, place2, RecordID, Reference);
                Btree.seekp(0, ios::beg);
                Btree.write((char *) &header, sizeof(root));
                Btree.seekp(sizeof(header), ios::beg);
                Btree.write((char *) &root, sizeof(root));
                Btree.seekp(sizeof(header) * place1, ios::beg);
                Btree.write((char *) &node1, sizeof(root));
                Btree.seekp(sizeof(header) * place2, ios::beg);
                Btree.write((char *) &node2, sizeof(root));
                Btree.close();

            }
        } else {

            /*
            1 awl e7timal nh ha insert w azbot l max
            2 tany e7timal ny atr a split l leaf
              tany e7timal 3ndo e7tmalen (
            3 awl e7timal nh l parent malyan
            4 tany e7timal nh l parent fady
                )
            3 3ndo e7tmalen
            5 nh l parent non leaf
            6 nh l parent l root
             */

            //multi-level bTree
            pair<int, vector<int>> visited = getLeaf(filename, root, RecordID);
            int indx = visited.first;
            Btree.seekg(indx * sizeof(root), ios::beg);
            node target;
            Btree.read((char *) &target, sizeof(root));
            int i, flag = 0;
            for (i = 0; i < target.size; i += 2) {
                if (target.val[i] == -1) {
                    flag = 1;
                    target.val[i] = RecordID;
                    target.val[i + 1] = Reference;
                    sortNode(target.val, i + 1);
                    Btree.seekp(indx * sizeof(root), ios::beg);
                    Btree.write((char *) &target, sizeof(root));
                    break;
                }
            }
            if (flag == 1) {
                for (auto &it: visited.second) {
                    Btree.seekp(it * sizeof(root), ios::beg);
                    Btree.read((char *) &target, sizeof(root));
                    if (target.nonLeaf) {
                        int j;
                        for (j = 0; j < target.size; j += 2) {
                            if (target.val[j] == -1) {
                                j -= 2;
                                target.val[j] = RecordID;
                                target.val[j + 1] = indx;
                                break;
                            }
                        }
                    }
                }
                Btree.close();
                return 0;
            } else {
                // here we need to split the leaf
                // we got two ways
                /// the parent of this leaf is root so we go check if root has place to split if it has we split leaf then insert again
                /// if not then we split root then call insert again which splits leaf then call insert again which inserts
                /// if the parent is not root then we check if it has place to split in  if it has we split if it doesnt we split its parent
                /// and so on every time after we split we call insert

                if (flag == 0 && visited.second.size() == 2) {
                    //take empty node
                    int place1 = header.val[0];
                    Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                    node node1;
                    Btree.read((char *) &node1, sizeof(node1));
                    header.val[0] = node1.val[0];
                    //split
                    if (!splitLeaf(root, target, node1, place1)) {
                        int place2 = header.val[0];
                        Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                        node node2;
                        Btree.read((char *) &node2, sizeof(node1));
                        header.val[0] = node2.val[0];

                        splitRoot(root,node1,node2,place1,place2);

                        int place3 = header.val[0];
                        Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                        node node3;
                        Btree.read((char *) &node3, sizeof(node1));
                        header.val[0] = node3.val[0];

                        splitLeaf(root,target,node3,place3);
                        Btree.close();
                        InsertNewRecordAtIndex(filename,RecordID,Reference);
                    }
                }
            }
            try {
                Btree.close();
            }
            catch (exception e){

            }

            return 0;
        }

    }


    return -1;
}

int SearchARecord(const char *filename, int RecordID) {
    fstream Btree;
    Btree.open(filename, ios::in);
    Btree.seekg(0, ios::beg);
    node header;
    Btree.read((char *) &header, sizeof(header));
    Btree.seekg(sizeof(header), ios::beg);
    node root;
    Btree.read((char *) &root, sizeof(root));
    int ans = -1, target = -1;
    if (!root.nonLeaf) {
        for (int i = 0; i < root.size; i += 2) {
            if (root.val[i] == RecordID)
                ans = root.val[i + 1];
        }
    } else {
        bool flag = false, entered = false;
        for (int i = 0; i < root.size and root.val[i] != -1; i += 2) {
            if (root.val[i] >= RecordID) {
                target = root.val[i + 1];
                Btree.seekg(target * sizeof(root), ios::beg);
                node n;
                Btree.read((char *) &n, sizeof(root));
                while (n.nonLeaf) {
                    entered = true;
                    int j;
                    for (j = 0; j < n.size and n.val[i] != -1; j += 2) {
                        if (n.val[i] >= RecordID) {
                            target = n.val[i + 1];
                            Btree.seekg(target * sizeof(root), ios::beg);
                            Btree.read((char *) &n, sizeof(root));
                            flag = true;
                            break;
                        }
                    }
                    if (flag) {
                        for (int k = 0; k < n.size; k += 2) {
                            if (RecordID == n.val[i])
                                ans = n.val[i + 1];
                        }
                    }
                }
                if (!entered) {
                    for (int k = 0; k < n.size; k += 2) {
                        if (RecordID == n.val[k])
                            ans = n.val[k + 1];
                    }
                }
            }

        }
    }
    Btree.close();
    return ans;
}


int main() {
    string name = "btree.bin";
    const char *filename = name.c_str();
    CreateIndexFileFile(filename, 10, 5);
    DisplayIndexFileContent(filename);
    InsertNewRecordAtIndex(filename, 3, 12);
    InsertNewRecordAtIndex(filename, 7, 24);
    InsertNewRecordAtIndex(filename, 10, 48);
    InsertNewRecordAtIndex(filename, 24, 60);
    InsertNewRecordAtIndex(filename, 14, 72);
    DisplayIndexFileContent(filename);
    InsertNewRecordAtIndex(filename, 19, 84);
    DisplayIndexFileContent(filename);
    InsertNewRecordAtIndex(filename, 15, 108);
    DisplayIndexFileContent(filename);
    InsertNewRecordAtIndex(filename, 1, 120);
    DisplayIndexFileContent(filename);
    InsertNewRecordAtIndex(filename, 5, 132);
    DisplayIndexFileContent(filename);
    InsertNewRecordAtIndex(filename, 30, 96);

    DisplayIndexFileContent(filename);
    cout << SearchARecord(filename, 5) << endl;
    cout << SearchARecord(filename, 1) << endl;
    cout << SearchARecord(filename, 20) << endl;
    cout << SearchARecord(filename, 19) << endl;

    return 0;
}
