// Node.h
#ifndef NODE_H
#define NODE_H

#include <vector>

class Node {
public:
    // basic structure of a tree node
    Node* leftChild;
    Node* rightChild;
    bool leaf = false;
    std::vector<int> data_id_list;      // data covered by **leaf node**

    // exclusive to ball tree node
    std::vector<int> farthest_point_id;
    std::vector<double> pivot;
    double radius;

    // used for dask-means
    std::vector<double> sum_vector;
    int point_number = 0;
    int centroid_id = -1;   // internal: centroid id the the node is assigned to
    std::vector<int> centroid_id_for_data;  // leaf: centroid id that each data is assiged to
    double ub = 0.0;

public:
    Node();

    Node(std::vector<double> pivot, double radius);

    Node(std::vector<double> pivot, double radius, bool isLeaf);

    ~Node();

    bool isLeaf() { return this->leaf; }

    void initLeafNode(std::vector<int> data_id_list, int size);

    void setSum();

    std::vector<int> getAllDataId();

    bool operator==(const Node& other) const;
};

#endif // NODE_H