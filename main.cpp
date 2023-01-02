#include <bits/stdc++.h>
#include "node.h"

using namespace std;

/// Declarations
pair<int, vector<int>> getLeaf(const char *filename, node &root, int id);

void CreateIndexFileFile(const char *filename, int numberOfRecords, int m);

void DisplayIndexFileContent(const char *filename);

int SearchARecord(const char *filename, int RecordID);

void sortNode(int *arr, int indx);

void splitRootNLeaf(node &root, node &node1, node &node2, int place1, int place2,
                    int max1, int max2, int oldMax, int max1place, int max2place);

void splitRootLeaf(node &root, node &node1, node &node2, int place1, int place2, int id = -1, int ref = -1);

pair<int, int> splitLeaf(node &tosplit, node &splitIn, int RecordID = -1, int ref = -1);


int splitParent(node &parent, node &Gparent, int Gparentindx, node &splitIn, int tosplitindx, int max1, int max2,
                int oldMax, int max1place, int max2place);

int InsertNewRecordAtIndex(const char *filename, int RecordID, int Reference);

pair<int,vector<int>> searchToDelete(const char *filename, int RecordID);

pair<int,int> deleteFromNode(node& ToDeleteFrom,int RecordID);

void DeleteRecordFromIndex (const char * filename, int RecordID);

pair<int,int> mergeNodes(node& node1,node& node2,int size1, int size2);
/// Declarations

pair<int, vector<int>> getLeaf(const char *filename, node &root, int id) {
    fstream Btree;
    Btree.open(filename, ios::in);
    vector<int> v;
    int target = -1, i;
    bool flag = false;
    v.push_back(1);
    for (i = 0; i < root.size and root.val[i] != -1; i += 2) {
        if (root.val[i] >= id) {
            flag = true;
            target = root.val[i + 1];
            v.push_back(target);
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
                    for (j = 0; j < n.size and n.val[j] != -1; j += 2) {
                        if (n.val[j] >= RecordID) {
                            target = n.val[j + 1];
                            Btree.seekg(target * sizeof(root), ios::beg);
                            Btree.read((char *) &n, sizeof(root));
                            flag = true;
                            break;
                        }
                    }
                    if (!flag) {
                        return -1;
                    }
                }
                if (flag) {
                    for (int k = 0; k < n.size; k += 2) {
                        if (RecordID == n.val[k])
                            ans = n.val[k + 1];
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

void splitRootNLeaf(node &root, node &node1, node &node2, int place1, int place2,
                    int max1, int max2, int oldMax, int max1place, int max2place) {
    int i;
    node1.nonLeaf = 1;
    node2.nonLeaf = 1;
    vector<pair<int, int>> v;
    for (int x = 0; x < root.size; x += 2) {
        if (root.val[x] != max1 and root.val[x] != max2 and root.val[x] != oldMax)
            v.emplace_back(root.val[x], root.val[x + 1]);
    }
    v.emplace_back(max1, max1place);
    v.emplace_back(max2, max2place);
    sort(v.begin(), v.end());
    int j = 0;
    for (i = 0; i < (v.size() / 2); i++) {
        node1.val[j] = v[i].first;
        node1.val[j + 1] = v[i].second;
        j += 2;
    }
    int maxroot1 = node1.val[j - 2];
    j = 0;
    while (i < v.size()) {
        node2.val[j] = v[i].first;
        node2.val[j + 1] = v[i].second;
        j += 2;
        i++;
    }
    int maxroot2 = node2.val[j - 2];
    root.val[0] = maxroot1;
    root.val[1] = place1;
    root.val[2] = maxroot2;
    root.val[3] = place2;
    for (int k = 4; k < root.size; ++k) {
        root.val[k] = -1;
    }
}

void splitRootLeaf(node &root, node &node1, node &node2, int place1, int place2, int id, int ref) {
    int i;
    root.nonLeaf = 1;
    node1.nonLeaf = 0;
    node2.nonLeaf = 0;
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

pair<int, int> splitLeaf(node &tosplit, node &splitIn, int RecordID, int ref) {
    splitIn.nonLeaf = 0;
    vector<pair<int, int>> v;
    for (int x = 0; x < tosplit.size; x += 2) {
        v.emplace_back(tosplit.val[x], tosplit.val[x + 1]);
    }
    v.emplace_back(RecordID, ref);
    sort(v.begin(), v.end());
    int j = 0, i;
    for (i = 0; i < (v.size() / 2); i++) {
        tosplit.val[j] = v[i].first;
        tosplit.val[j + 1] = v[i].second;
        j += 2;
    }
    int max1 = tosplit.val[j - 2];
    while (j < tosplit.size) {
        tosplit.val[j] = -1;
        tosplit.val[++j] = -1;
        ++j;
    }
    j = 0;
    for (int i = v.size() / 2; i < v.size(); i++) {
        splitIn.val[j] = v[i].first;
        splitIn.val[j + 1] = v[i].second;
        j += 2;
    }
    int max2 = splitIn.val[j - 2];
    pair<int, int> split_result;
    split_result = {max1, max2};
    return split_result;
}

pair<int, int> splitParent(node &parent, node &splitIn, int splitInindex, int max1, int max2,
                           int oldMax, int max1place, int max2place) {

    splitIn.nonLeaf = 1;
    vector<pair<int, int>> v;
    for (int x = 0; x < parent.size; x += 2) {
        if (parent.val[x] != max1 and parent.val[x] != max2 and parent.val[x] != oldMax)
            v.emplace_back(parent.val[x], parent.val[x + 1]);
    }
    v.emplace_back(max1, max1place);
    v.emplace_back(max2, max2place);
    sort(v.begin(), v.end());
    int j = 0, i;
    for (i = 0; i < (v.size() / 2); i++) {
        parent.val[j] = v[i].first;
        parent.val[j + 1] = v[i].second;
        j += 2;
    }
    max1 = parent.val[j - 2];
    while (j < splitIn.size) {
        parent.val[j] = -1;
        parent.val[++j] = -1;
        ++j;
    }
    j = 0;
    for (int i = v.size() / 2; i < v.size(); i++) {
        splitIn.val[j] = v[i].first;
        splitIn.val[j + 1] = v[i].second;
        j += 2;
    }
    max2 = splitIn.val[j - 2];
    pair<int, int> split_result;
    split_result = {max1, max2};
    return split_result;
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
        return 1;
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
                splitRootLeaf(root, node1, node2, place1, place2, RecordID, Reference);
                Btree.seekp(0, ios::beg);
                Btree.write((char *) &header, sizeof(root));
                Btree.seekp(sizeof(header), ios::beg);
                Btree.write((char *) &root, sizeof(root));
                Btree.seekp(sizeof(header) * place1, ios::beg);
                Btree.write((char *) &node1, sizeof(root));
                Btree.seekp(sizeof(header) * place2, ios::beg);
                Btree.write((char *) &node2, sizeof(root));
                Btree.close();
                return 0;
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
                                if (target.val[j] < RecordID) {
                                    target.val[j] = RecordID;
                                    target.val[j + 1] = indx;
                                }
                                break;
                            }
                        }
                    }
                }
                Btree.close();
                return 0;
            }
            else {
                // here we need to split the leaf
                // we got two ways
                /// the parent of this leaf is root so we go check if root has place to split if it has we split leaf then insert again -> done
                /// if not then we split root then call insert again which splits leaf then call insert again which inserts -> done
                /// if the parent is not root then we check if it has place to split in  if it has we split if it doesnt we split its parent
                /// and so on every time after we split we call insert


                ///split the leaf into two leaves
                //take empty node
                int place1 = header.val[0];
                Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                node node1;
                Btree.read((char *) &node1, sizeof(node1));
                header.val[0] = node1.val[0];
                //split
                int oldMax = target.val[target.size - 2];
                pair<int, int> ans = splitLeaf(target, node1, RecordID, Reference);

                Btree.seekp(place1 * sizeof(header), ios::beg);
                Btree.write((char *) &node1, sizeof(header));

                vector<pair<int, int>> toUse;
                toUse.emplace_back(ans.first, indx);
                toUse.emplace_back(ans.second, place1);

                /// if my parent is root
                if (visited.second.size() == 2) {
                    /// if root is empty
                    if (root.val[root.size - 1] == -1) {
                        vector<pair<int, int>> v;
                        for (int x = 0; x < root.size; x += 2) {
                            if (root.val[x] != toUse[0].first and root.val[x] != toUse[1].first and
                                root.val[x] != oldMax and root.val[x] != -1)
                                v.emplace_back(root.val[x], root.val[x + 1]);
                        }
                        v.emplace_back(toUse[0].first, toUse[0].second);
                        v.emplace_back(toUse[1].first, toUse[1].second);
                        sort(v.begin(), v.end());
                        int j = 0;
                        for (i = 0; i < v.size(); i++) {
                            root.val[j] = v[i].first;
                            root.val[j + 1] = v[i].second;
                            j += 2;
                        }
                        while (j < root.size) {
                            root.val[j] = -1;
                            root.val[++j] = -1;
                            ++j;
                        }

                    }
                        /// if root not empty split root then put the split leaves
                    else {
                        int place2 = header.val[0];
                        Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                        node node2;
                        Btree.read((char *) &node2, sizeof(node1));
                        header.val[0] = node2.val[0];

                        int place3 = header.val[0];
                        Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                        node node3;
                        Btree.read((char *) &node3, sizeof(node1));
                        header.val[0] = node3.val[0];

                        splitRootNLeaf(root, node2, node3, place2, place3, toUse[0].first, toUse[1].first, oldMax,
                                       toUse[0].second, toUse[1].second);

                        Btree.seekp(place3 * sizeof(header), ios::beg);
                        Btree.write((char *) &node3, sizeof(header));

                        Btree.seekp(place2 * sizeof(header), ios::beg);
                        Btree.write((char *) &node2, sizeof(header));
                    }
                    Btree.close();
                    return 0;
                } else {

                    int parentindx = visited.second[visited.second.size() - 1];
                    Btree.seekg(parentindx * sizeof(header), ios::beg);
                    node parent;
                    Btree.read((char *) &parent, sizeof(parent));

                    /// if parent is empty
                    if (parent.val[parent.size - 1] == -1) {
                        vector<pair<int, int>> v;
                        for (int x = 0; x < parent.size; x += 2) {
                            if (parent.val[x] != toUse[0].first and parent.val[x] != toUse[1].first and
                                parent.val[x] != oldMax and parent.val[x] != -1)
                                v.emplace_back(parent.val[x], parent.val[x + 1]);
                        }
                        v.emplace_back(toUse[0].first, toUse[0].second);
                        v.emplace_back(toUse[1].first, toUse[1].second);
                        sort(v.begin(), v.end());
                        int j = 0;
                        for (i = 0; i < v.size(); i++) {
                            parent.val[j] = v[i].first;
                            parent.val[j + 1] = v[i].second;
                            j += 2;
                        }
                        while (j < parent.size) {
                            parent.val[j] = -1;
                            parent.val[++j] = -1;
                            ++j;
                        }

                    }
                        /// if parent not empty split parent then put split leaves in
                    else {
                        bool flag=false;
                        for (int i = visited.second.size() - 2; i >=0; i--) {

                            Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                            node node4;
                            Btree.read((char *) &node4, sizeof(node4));
                            int place4 = header.val[0];
                            header.val[0] = node4.val[0];

                            int parentMax=parent.val[parent.size-2];
                            pair<int, int> ans = splitParent(parent, node4, place4, toUse[0].first, toUse[1].first,
                                                             oldMax, toUse[0].second, toUse[1].second);
                            Btree.seekg(place4* sizeof(header), ios::beg);
                            Btree.write((char *) &node4, sizeof(node4));

                            toUse[0].first = ans.first;
                            toUse[1].first = ans.second;
                            toUse[0].second=visited.second[i+1];
                            toUse[1].second=place4;
                            oldMax=parentMax;
                            Btree.seekg(visited.second[i]* sizeof(header), ios::beg);
                            Btree.read((char *) &parent, sizeof(parent));
                            if (parent.val[parent.size - 1] == -1){
                                flag=true;
                                break;
                            }
                        }
                        if(flag){
                            vector<pair<int, int>> v;
                            for (int x = 0; x < parent.size; x += 2) {
                                if (parent.val[x] != toUse[0].first and parent.val[x] != toUse[1].first and
                                    parent.val[x] != oldMax and parent.val[x] != -1)
                                    v.emplace_back(parent.val[x], parent.val[x + 1]);
                            }
                            v.emplace_back(toUse[0].first, toUse[0].second);
                            v.emplace_back(toUse[1].first, toUse[1].second);
                            sort(v.begin(), v.end());
                            int j = 0;
                            for (i = 0; i < v.size(); i++) {
                                parent.val[j] = v[i].first;
                                parent.val[j + 1] = v[i].second;
                                j += 2;
                            }
                            while (j < parent.size) {
                                parent.val[j] = -1;
                                parent.val[++j] = -1;
                                ++j;
                            }
                            /// if i insert and splitted parents in the middle so I'm not sure if up their in the tree they have the max value
                            for (auto &it: visited.second) {
                                Btree.seekp(it * sizeof(root), ios::beg);
                                Btree.read((char *) &target, sizeof(root));
                                if (target.nonLeaf) {
                                    int j;
                                    for (j = 0; j < target.size; j += 2) {
                                        if (target.val[j] == -1) {
                                            j -= 2;
                                            if (target.val[j] < RecordID) {
                                                target.val[j] = RecordID;
                                                target.val[j + 1] = indx;
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }else{
                            int place2 = header.val[0];
                            Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                            node node2;
                            Btree.read((char *) &node2, sizeof(node1));
                            header.val[0] = node2.val[0];

                            int place3 = header.val[0];
                            Btree.seekg(header.val[0] * sizeof(header), ios::beg);
                            node node3;
                            Btree.read((char *) &node3, sizeof(node1));
                            header.val[0] = node3.val[0];

                            splitRootNLeaf(parent, node2, node3, place2, place3, toUse[0].first, toUse[1].first, oldMax,
                                           toUse[0].second, toUse[1].second);

                            Btree.seekp(place3 * sizeof(header), ios::beg);
                            Btree.write((char *) &node3, sizeof(header));

                            Btree.seekp(place2 * sizeof(header), ios::beg);
                            Btree.write((char *) &node2, sizeof(header));
                        }
                        Btree.close();
                        return 0;
                    }

                }
            }
        }
    }
    /// return -1 if Btree is full and has no more empty nodes "header is pointing on -1"
    return -1;
}

pair<int,vector<int>> searchToDelete(const char *filename, int RecordID) {
    pair<int,vector<int>>visited;
    fstream Btree;
    Btree.open(filename, ios::in);
    Btree.seekg(0, ios::beg);
    node header;
    Btree.read((char *) &header, sizeof(header));
    Btree.seekg(sizeof(header), ios::beg);
    node root;
    Btree.read((char *) &root, sizeof(root));
    if(header.val[0] == -1 and root.nonLeaf == -1)
    {
        visited.first = -1;
        return visited;
    }
    visited.second.push_back(1);
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
                visited.second.push_back(target);
                Btree.seekg(target * sizeof(root), ios::beg);
                node n;
                Btree.read((char *) &n, sizeof(root));
                while (n.nonLeaf) {
                    entered = true;
                    int j;
                    for (j = 0; j < n.size and n.val[j] != -1; j += 2) {
                        if (n.val[j] >= RecordID) {
                            target = n.val[j + 1];
                            Btree.seekg(target * sizeof(root), ios::beg);
                            Btree.read((char *) &n, sizeof(root));
                            flag = true;
                            if(n.nonLeaf==0)
                                visited.second.push_back(target);
                            break;
                        }
                    }
                    if (!flag) {
                       visited.first = -1;
                        return visited;
                    }
                }
                if (flag) {
                    for (int k = 0; k < n.size; k += 2) {
                        if (RecordID == n.val[k])
                        {
                            visited.first = k;
                            return visited;
                        }

                    }
                }
                if (!entered) {
                    for (int k = 0; k < n.size; k += 2) {
                        if (RecordID == n.val[k])
                        {
                            visited.first = k;
                            return visited;
                        }
                    }
                }
            }

        }
    }
    Btree.close();
    return visited;
}

pair<int,int> deleteFromNode(node& ToDeleteFrom,int RecordID){
    vector<pair<int,int>>v;
    for (int i = 0; i < ToDeleteFrom.size and ToDeleteFrom.val[i]!=-1; i+=2) {
        if(ToDeleteFrom.val[i]!=RecordID )
            v.push_back({ToDeleteFrom.val[i],ToDeleteFrom.val[i+1]});
    }
    int j = 0,max = -1;
    for (int i = 0; i < v.size(); ++i) {
        ToDeleteFrom.val[j] = v[i].first;
        ToDeleteFrom.val[j+1] = v[i].second;
        max = v[i].first;
        j+=2;
    }
    while(j<ToDeleteFrom.size)
    {
        ToDeleteFrom.val[j] = -1;
        ToDeleteFrom.val[j+1] = -1;
        j+=2;
    }
    return {max,v.size()};
}

void DeleteRecordFromIndex (const char * filename, int RecordID) {
    pair<int,vector<int>> visited = searchToDelete(filename,RecordID);
    fstream Btree;
    Btree.open(filename, ios::in | ios::out | ios::binary);
    node header;
    Btree.seekg(0,ios::beg);
    Btree.read((char*)&header,sizeof(header));
    if(visited.first==-1)
    {
        cout<<"Record not found"<<endl;
        return;
    }
    int indxofNode = visited.second[visited.second.size()-1];
    node ToDeleteFrom;
    Btree.seekg(indxofNode*sizeof(ToDeleteFrom),ios::beg);
    Btree.read((char*)&ToDeleteFrom,sizeof(ToDeleteFrom));
    pair<int,int>result = deleteFromNode(ToDeleteFrom,RecordID);
    if(visited.second.size()==1)
    {
        if(ToDeleteFrom.val[0]==-1)
        {
            ToDeleteFrom.val[0] = header.val[0];
            header.val[0] = 1;
        }
        return;
    }
    /// Delete with no underflow
    if(result.second>= floor(ToDeleteFrom.size/4))
    {
        if(result.first<RecordID){
            ///write new max for all visited nodes
            node target;
            for (auto &it: visited.second) {
                Btree.seekp(it * sizeof(ToDeleteFrom), ios::beg);
                Btree.read((char *) &target, sizeof(ToDeleteFrom));
                if (target.nonLeaf) {
                    int j;
                    for (j = 0; j < target.size; j += 2) {
                        if (target.val[j]== RecordID) {
                            target.val[j] = result.first;
                            break;
                        }
                    }
                }
            }
            return;
        }
        else
            return;

    }
    else{
        ///underflow condition
        node parent;
        int indxOfParent = visited.second[visited.second.size()-2];
        Btree.seekg(indxOfParent*sizeof(ToDeleteFrom));
        Btree.read((char*)&parent,sizeof(parent));
        int indxInParent = 0;
        for(int i = 0 ; i < parent.size; i+=2)
        {
            if(parent.val[i]>=RecordID)
            {
                indxInParent = i;
                break;
            }
        }
        node siblingB, siblingA;
        int indxB,indxA;
        /// if there is a right sibling
        if(indxInParent-2>=0)
        {
            indxB = parent.val[indxInParent-1];
            Btree.seekg(indxB*sizeof(ToDeleteFrom));
            Btree.read((char*)&siblingB,sizeof(siblingB));
        }
        ///if there is a left sibling
        if(parent.val[indxInParent+2]!=-1)
        {
            indxA = parent.val[indxInParent+3];
            Btree.seekg(indxA*sizeof(ToDeleteFrom));
            Btree.read((char*)&siblingA,sizeof(siblingA));
        }
        int sizeB = 0, sizeA = 0;
        for (int i = 0; i < siblingB.size; i+=2) {
            if(siblingB.val[i]!=-1)
                sizeB++;
            else
                break;
        }
        for (int i = 0; i < siblingA.size; i+=2) {
            if(siblingA.val[i]!=-1)
                sizeA++;
            else
                break;
        }
        bool redistribute = false;
        /// if taking from left sibling doesn't cause underflow
        if(sizeB-1>=floor(siblingB.size/4) and siblingB.nonLeaf!=-1)
        {
            int newElement = siblingB.val[sizeB*2-2];
            int newRef = siblingB.val[sizeB*2-1];
            int newMaxB = siblingB.val[sizeB*2-4];
            siblingB.val[sizeB*2-2] = -1;
            siblingB.val[sizeB*2-1] = -1;
            for (int i = 0; i < ToDeleteFrom.size; i+=2) {
                if(ToDeleteFrom.val[i]==-1)
                {
                    ToDeleteFrom.val[i]= newElement;
                    ToDeleteFrom.val[i+1] = newRef;
                    sortNode(ToDeleteFrom.val,i+1);
                    redistribute = true;
                    break;
                }
            }
            /// fix max values among all levels visited
            node target;
            for (auto &it: visited.second) {
                Btree.seekp(it * sizeof(ToDeleteFrom), ios::beg);
                Btree.read((char *) &target, sizeof(ToDeleteFrom));
                if (target.nonLeaf) {
                    int j;
                    for (j = 0; j < target.size; j += 2) {
                        if (target.val[j]== RecordID) {
                            target.val[j] = result.first;
                        }
                        if(target.val[j] == newElement) {
                            target.val[j] = newMaxB;
                        }
                    }
                }
            }
            return;
        }
        /// if taking from right sibling doesn't cause underflow
        else if(sizeA-1>=floor(siblingA.size/4) and siblingB.nonLeaf!=-1 and !redistribute){
            int newElement = siblingA.val[0];
            int newRef = siblingA.val[1];
            deleteFromNode(siblingA,newElement);
            for (int i = 0; i < ToDeleteFrom.size; i+=2) {
                if(ToDeleteFrom.val[i]==-1)
                {
                    ToDeleteFrom.val[i]= newElement;
                    ToDeleteFrom.val[i+1] = newRef;
                    sortNode(ToDeleteFrom.val,i+1);
                    redistribute = true;
                    break;
                }
            }
            node target;
            for (auto &it: visited.second) {
                Btree.seekp(it * sizeof(ToDeleteFrom), ios::beg);
                Btree.read((char *) &target, sizeof(ToDeleteFrom));
                if (target.nonLeaf) {
                    int j;
                    for (j = 0; j < target.size; j += 2) {
                        if (target.val[j]== result.first) {
                            target.val[j] = newElement;
                            break;
                        }

                    }
                }
            }
            return;
        }
        /// merge
        else{
            if(siblingB.nonLeaf!=-1)
            {
                pair<int,int>mergeOldMax =mergeNodes(siblingB,ToDeleteFrom,sizeB,result.second);
                Btree.seekp(indxofNode*sizeof(ToDeleteFrom),ios::beg);
                ToDeleteFrom.val[0] = header.val[0];
                header.val[0] = indxofNode;
                Btree.write((char*)&ToDeleteFrom,sizeof(ToDeleteFrom));
                if(result.first<RecordID)
                {
                    node target;
                    for (int i = 0; i < visited.second.size()-2; i++) {
                        Btree.seekp(visited.second[i] * sizeof(ToDeleteFrom), ios::beg);
                        Btree.read((char *) &target, sizeof(ToDeleteFrom));
                        if (target.nonLeaf) {
                            int j;
                            for (j = 0; j < target.size; j += 2) {
                                if (target.val[j]==RecordID ) {
                                    target.val[j] = result.first;
                                    break;
                                }

                            }
                        }
                    }
                }
                int parentIndex = visited.second[visited.second.size()-2];
                node parentNode;
                Btree.seekp(parentIndex * sizeof(parentNode), ios::beg);
                Btree.read((char *) &parentNode, sizeof(parentNode));
                for (int i = 1; i < parentNode.size; i+=2) {
                    if(parentNode.val[i] == indxofNode){
                        parentNode.val[i] = -1;
                        parentNode.val[i-1] = -1;
                        parentNode.val[i-3] = result.first;
                        break;
                    }
                }


            }
            else{
                mergeNodes(ToDeleteFrom,siblingA,result.second,sizeA);
                Btree.seekp(indxA*sizeof(siblingA),ios::beg);
                siblingA.val[0] = header.val[0];
                header.val[0] = indxA;
                Btree.write((char*)&siblingA,sizeof(siblingA));
                if(result.first<RecordID)
                {
                    node target;
                    for (int i = 0; i < visited.second.size()-2; i++) {
                        Btree.seekp(visited.second[i] * sizeof(ToDeleteFrom), ios::beg);
                        Btree.read((char *) &target, sizeof(ToDeleteFrom));
                        if (target.nonLeaf) {
                            int j;
                            for (j = 0; j < target.size; j += 2) {
                                if (target.val[j]==RecordID ) {
                                    target.val[j] = result.first;
                                    break;
                                }

                            }
                        }
                    }
                }
                int parentIndex = visited.second[visited.second.size()-2];
                node parentNode;
                Btree.seekp(parentIndex * sizeof(parentNode), ios::beg);
                Btree.read((char *) &parentNode, sizeof(parentNode));
                for (int i = 1; i < parentNode.size; i+=2) {
                    if(parentNode.val[i] == indxA){
                        parentNode.val[i] = -1;
                        parentNode.val[i-1] = -1;
                        parentNode.val[i-3] = result.first;
                    }
                }
            }
        }
    }

}
pair<int,int> mergeNodes(node& node1,node& node2,int size1, int size2){
    int max1 = node1.val[size1*2-2];
    int max2 = node2.val[size2*2-2];
    for (int i = size1*2, j =0; i < node1.size and j<size2*2; ++i,j++) {
        node1.val[i] = node2.val[j];
    }
    for(int i = 0; i<size2*2;i++)
        node2.val[i] = -1;
    node2.nonLeaf = -1;
    return{max1,max2};
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

    InsertNewRecordAtIndex(filename, 30, 96);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 15, 108);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 1, 120);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 5, 132);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 2, 75);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 8, 156);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 9, 168);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 6, 180);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 11, 192);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 12, 204);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 17, 217);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 18, 228);
    DisplayIndexFileContent(filename);

    InsertNewRecordAtIndex(filename, 32, 240);
    DisplayIndexFileContent(filename);

    DeleteRecordFromIndex(filename,10);
    DisplayIndexFileContent(filename);

    DeleteRecordFromIndex(filename,9);
    DisplayIndexFileContent(filename);

    DeleteRecordFromIndex(filename,8);
    DisplayIndexFileContent(filename);


    cout << SearchARecord(filename, 5) << endl;
    cout << SearchARecord(filename, 1) << endl;
    cout << SearchARecord(filename, 20) << endl;
    cout << SearchARecord(filename, 19) << endl;
    cout << SearchARecord(filename, 1) << endl;
    cout << SearchARecord(filename, 10) << endl;
    return 0;
}
