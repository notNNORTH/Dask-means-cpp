#include "KMeansBase.h"
#include "Centroid.h"
#include <iostream>
#include <random>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <Utils.h>

using namespace Utils;
using namespace std;

KMeansBase::KMeansBase(int max_iterations, double convergence_threshold)
    : max_iterations(max_iterations), convergence_threshold(convergence_threshold) {}

KMeansBase::~KMeansBase() {
    for (Centroid* centroid : centroid_list) { delete centroid; }
}

void KMeansBase::initParameters(int data_scale, int data_dimension, int k) {
    this->data_scale = data_scale;
    this->data_dimension = data_dimension;
    this->k = k;
    this->labels.assign(this->data_scale, 0);
}

void KMeansBase::setK(int k) {
    this->k = k;
}

// load data into para::dataset
void KMeansBase::load(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return;
    }

    dataset.clear();
    std::string line;
    int count = 0;

    while (std::getline(file, line)) {
        if (count >= data_scale) break;

        std::stringstream ss(line);
        std::vector<double> data_point;
        double value;

        while (ss >> value) {
            data_point.push_back(value);
            if (ss.peek() == ',' || ss.peek() == ' ') ss.ignore();
        }

        if (data_point.size() == data_dimension) {
            dataset.push_back(data_point);
            count++;
        } else {
            std::cerr << "Warning: Ignoring line with incorrect dimension: " << line << std::endl;
        }
    }

    file.close();
}

// output k cluster with the schema as {cluster_id: int, point_id_list: list()}
void KMeansBase::output(const std::string& file_path) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }

    for (auto centroid : centroid_list) {
        Cluster* clus = centroid->getCluster();
        file << "{cluster_id: " << clus->getClusterId() << ", point_id_list: [";

        std::vector<int> data_id_list = clus->getDataIdList();
        for (size_t i = 0; i < data_id_list.size(); ++i) {
            file << data_id_list[i];
            if (i < data_id_list.size() - 1) {
                    file << ", ";
            }
        }
        file << "]}" << std::endl;
    }

    file.close();
}

// initialize all centroids randomly
void KMeansBase::initializeCentroids() {
    std::set<int> selected_ids;

    for (int i; i < k; i ++) {
        // unsigned seed = 42;      // fixed seed
        std::random_device seed;    // random seed
        std::mt19937 gen(seed());
        std::uniform_int_distribution<> distrib(0, data_scale - 1);

        int point_id;
        do {
            point_id = distrib(gen);
        } while (selected_ids.find(point_id) != selected_ids.end());

        selected_ids.insert(point_id);
        Centroid* centroid = new Centroid(point_id, dataset[point_id], i);

        centroid_list.push_back(centroid);
    }
}

// assign all data points to a cluster with an intiger label
void KMeansBase::assignLabels() {}

// update each centroid as the mean of each cluster
void KMeansBase::updateCentroids() {
    for (auto centroid : centroid_list) {
        std::vector<int> data_id_list = centroid->getCluster()->getDataIdList();
        int data_num = data_id_list.size();

        std::vector<double> new_coordinate(data_dimension, 0);
        for (int data_id : data_id_list) {
            new_coordinate = addVector(new_coordinate, dataset[data_id]);
        }
        new_coordinate = divideVector(new_coordinate, data_num);

        centroid->updateCoordinate(new_coordinate);
    }
}

// determine whether the algorithm has converged
bool KMeansBase::hasConverged() {
    double sum_drift = 0.0;
    
    for (auto centroid : centroid_list) {
        sum_drift += distance1(centroid->getCoordinate(), centroid->getOldCoordinate());
    }

    cout << "sum drift: " << sum_drift << endl;
    return sum_drift <= convergence_threshold;
}
