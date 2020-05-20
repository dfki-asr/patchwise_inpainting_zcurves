#pragma once

#include "definitions.h"

class IDistance
{
public:
    IDistance( std::vector<unsigned int>& ids, std::vector<float>& data, int numberOfPoints, int numberOfDimensions);
    ~IDistance();
    
    vector<int> QueryKNN( std::vector<float>& q, int num);
    
protected:
    vector<int> KNN_KEEP(float q[], int num);
    vector<int> KNN_RESTART(float q[], int num);
    vector<int> KNN_SEQSEARCH(float q[], int num);       
    
    //knn helper functions
    void SearchO_RESTART(float q[], float r, bool restrict_num);
    void SearchInward_RESTART(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool restrict_num); //left
    void SearchOutward_RESTART(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool restrict_num); //right
    void AddNeighbor_RESTART(int indexID, float distnode, bool sort, bool restrict_num);
    
    void SearchO_KEEP(float q[], float r, bool restrict_num);
    btree_mm::iterator SearchInward_KEEP(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool restrict_num); //left
    btree_mm::iterator SearchOutward_KEEP(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool restrict_num); //right
    void AddNeighbor_KEEP(int indexID, float distnode, bool sort);
    
    
public:
    //class helper functions
    void setData( std::vector<unsigned int>& ids, std::vector<float>& data, int numberOfPoints, int numberOfDimensions );
    void buildTree();
    void buildIndexValues();
    void buildReferencePoints(int num);
    void buildReferencePoints();
    void buildSplits();
    
protected:    
    void buildRefs_HALF_POINTS();
    void buildRefs_HALF_POINTS_CENTER();
    void buildRefs_HALF_POINTS_OUTSIDE();
    void buildRefs_RANDOM();
    void buildRefs_CLUSTER();
    void buildRefs_UNIFORM();
    void buildIndex_CLOSEST();
    void buildIndex_ASSIGN();
    void buildSplits_TOP_CENTERS();
    
public:
    void setReferencePoints(float* refs, int num);
    void saveTree(const char* fStr);
    void loadTree(const char* fStr);
    void initOptions();
    void setSequentialSearch(int s);
    void getStats(stats* theStats);
    void outputReferencePoints();

    void setSplitPoints(float* splits, int num);
    void setPointAssignment(float* assigns, int num);
    
    //options
    void setOptions(configOptions co);
    vector<int> getIDs(vector<int> indices);
    vector<float> getKNN_dists();
    vector<int> getKNN_nodes();
    float getKNN_avgDist();
    int calculateC(int d, int e);
    
    float calcIndex(int p, float c, float d);
    vector<int> getKNN_candidates();
    
    //Stats stuff
    template<typename T>
    void printVector(const std::vector<T>& vec, ostream &out, bool commas);
    float AverageDist(vector<float>& distances);
    void PrintNodeRange(ostream &out);
    void PrintNodeRange(const char* fp);
    void setQueryTime(float qTime);
    stats theStats;
    void resetNodeCount();
    int getNodeCount();
    void resetCandidateCount();
    int getCandidateCount();
    void resetPartitionCount();
    int getPartitionCount();  
    int getNumberOfPoints();
    int getNumberOfDimensions();
    
protected:
    float dist(float p1[], float p2[]);
    float roundOff(float in);
    float* findMedians(float* data, int p, int d, bool hasID);
    
    //the b+ tree
    btree_mm btree;
    
    //the nxd array storing our datapoints
	std::vector<float> datapoints;
    
    //the reference points for each partition
    float* reference_points;
    
    //the reference point to assign each data point
    //only used in certain configs
    float* point_assignments;
    
    //the max distance for each partition
    float* partition_dist_max;
    
    //the array index of the point that's the max dist for that partition
    unsigned int* partition_dist_max_index;
    
    //the ids for each data point
	std::vector<unsigned int> datapoint_ids;
    
    //the y index value for each data point
    float* datapoint_index;
    
    unsigned int number_partitions;
    unsigned int number_dimensions;
    unsigned int number_points;
    float constant_c;
    float radius_r;
    configOptions settings; //algorithm settings
    int doSeqSearch; // 0 = no, 1 = yes, 2 = verify
    int candidates;
    int partitionsChecked;
    
    //KNN Global Vars
    vector<int> knn_S;
    vector<float> knn_S_dists;
    vector<int> knn_S_nodes;
    vector<int> knn_S_candidates;
    
    unsigned int knn_K;
    bool knn_stopFlag;
    
    //Additional KNN_KEEP Global Vars
    float knn_pfarthest;
    int* knn_partition_checked;
    btree_mm::iterator* knn_left_pointers;
    btree_mm::iterator* knn_right_pointers;
};

