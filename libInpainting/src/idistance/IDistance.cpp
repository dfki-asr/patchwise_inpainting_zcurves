#include "stdafx.h"

#include <iostream>
#include <fstream>
#include "sys/times.h"
#include "string.h"
#include <math.h>
#include <numeric>

#include "IDistance.h"
   
IDistance::IDistance( std::vector<unsigned int>& ids, std::vector<float>& data, int n, int d)
	: datapoint_ids(ids)
{
	datapoint_index = NULL;
	partition_dist_max_index = NULL;
	partition_dist_max = NULL;
	reference_points = NULL;
	constant_c = 1; // basic default, not guaranteed separation
	doSeqSearch = 0; // basic default, don't do sequential search
	setData(ids, data, n, d);
	initOptions();
	buildIndexValues();
	buildTree();
}

IDistance::~IDistance()
{
    //delete allocated data
    if(datapoint_index != NULL)
    {
        delete [] datapoint_index;
        datapoint_index = NULL;
    }
    if(partition_dist_max_index != NULL)
    {
        delete [] partition_dist_max_index;
        partition_dist_max_index = NULL;
    }
    if(partition_dist_max != NULL)
    {
        delete [] partition_dist_max;
        partition_dist_max = NULL;
    }
    if(reference_points != NULL)
    {
        delete [] reference_points;
        reference_points = NULL;
    }
	btree.clear();
}

vector<int> IDistance::QueryKNN( std::vector<float>& q, int num )
{
    if ( doSeqSearch == 1 )
    {
        return KNN_SEQSEARCH(&q[0], num);
    }
      
    vector<int> results;
	switch(settings.knn_method)
    {
        case KEEP:
        {
            results = KNN_KEEP(&q[0], num);
        }
        break;
        case RESTART:
        {
            results = KNN_RESTART(&q[0], num);
        }
        break;
    }

    return getIDs(results);    
}

vector<int> IDistance::KNN_RESTART(float q[], int num)
{
    radius_r = settings.r_init;
    knn_stopFlag = false;
    knn_K = num;
    knn_S.clear();
    knn_S_dists.clear();
    knn_S_nodes.clear();
    knn_S_candidates.clear();
    
    //knn_pfarthest = 0;
        
    stx::CountQueryNodes("Start"); //reset query node count
    while(!knn_stopFlag)  //knn_ret.size() < num)//should probably have safety net (suppose num = 4, but only 3 points exist)
    {
        //increase radius (this could be a class parameter)
        radius_r += settings.r_delta;
        //check cross with each partition

        //cout << "searching radius : " << radius_r << endl;
        SearchO_RESTART(q, radius_r, true);
        
        //cout<<"********************************************"<<endl;
        //cout<<"With radius: " << radius_r << " KNN="<<knn_S.size()<<endl;
        if(radius_r > constant_c)   //assuming that constant_c is greater than max distance of space
            knn_stopFlag = true;
    }

    return knn_S;   
}

/********************************************************
 * Searches through all partitions for points within radius r of a 
 * query point q. If for true KNN, we are restricted on the number of
 * data points being retrieved.
 **************************/ 
void IDistance::SearchO_RESTART(float q[], float r, bool restrict_num)
{
    // set stop flag
    if(knn_S.size() == knn_K)  //also changed if our radius gets too big
        knn_stopFlag = true;
    
    //variable to store the distance from partition to query pt
    float distp;
    //the index value of the query point
    float q_index;
    
    partitionsChecked = 0;

	//check for each partition
    for ( unsigned int i=0; i < number_partitions; i++)
    {   
        //calc distance from query point to partition i
        distp = dist(&reference_points[i*number_dimensions],q);
        
        //calculate the q_index
        q_index = i*constant_c + distp;
        
        //check if query is interacting with partition (filter):
        //dist(O_i, q) - querydist(q)="r" <= dist_max_i
        if(distp - r <= partition_dist_max[i])
        {
            partitionsChecked++;
            //if query sphere is inside this partition
            if(distp <= partition_dist_max[i])
            {   
                //cout<<"Inside Partition "<<i<<endl;
                //find query pt and search in/out
                btree_mm::iterator lnode = btree.upper_bound(q_index);  
                btree_mm::iterator rnode;// = btree.lower_bound(q_index);
                rnode = lnode;
                SearchInward_RESTART(lnode, q_index - r, q, i, restrict_num);
                SearchOutward_RESTART(rnode--, q_index + r, q, i, restrict_num);
            }
            else //it intersects
            {
                //cout<<"Intersect Partition "<<i<<endl;
                //get the index value(y) of the pt of dist max
                btree_mm::iterator dist_max_i = btree.find(datapoint_index[partition_dist_max_index[i]]);
                SearchInward_RESTART(dist_max_i, q_index - r, q, i, restrict_num);
            }
        }
    }
}

/***************************************************
 * Search the B+ tree from the given node pointer inward (lower values)
 * until reaching the stopping value (ivalue)
 ***************************************/ 
void IDistance::SearchInward_RESTART(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool restrict_num)
{ 
    //variable for distance
    float distnode=constant_c;
    float partition_offset = roundOff(partition_i*constant_c);
    int past_data = -1;
    //i think these will be faster
    float node_key = node.key();
    int node_data = node.data();
    //cout<<"--SearchInward--"<<endl;
    //cout<<"Part_Offset: "<<partition_offset<<" iValue: "<<ivalue<<endl;
    //must get each node of that value  
    //while not to stopping value and still inside partition
    while(node_key >= ivalue && node_key >= partition_offset && node_data != past_data)
    {
        candidates++;
        distnode = dist(&datapoints[node_data*number_dimensions], q);
        //cout<<"INWARD KEY: "<<node.key()<<" DATA: "<<node.data()<<" DIST: "<<distnode<<endl;
        if(distnode <= radius_r)
        {
            AddNeighbor_RESTART(node_data, distnode, restrict_num, restrict_num);    //i'm assuming you only sort when you need to
        }
        past_data = node_data;
        node--;
        node_key = node.key();
        node_data = node.data();
    }
}

/***************************************************
 * Search the B+ tree from the given node pointer outward (higher values)
 * until reaching the stopping value (ivalue)
 ***************************************/ 
void IDistance::SearchOutward_RESTART(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool restrict_num)
{
    //variable for distance
    float distnode=constant_c;
    float dist_max = roundOff(partition_i*constant_c + partition_dist_max[partition_i]);
    int past_data = -1;
    //i think these will be faster
    float node_key = node.key();
    int node_data = node.data();
    //cout<<"--SearchOutward--"<<endl;
    //cout<<"Dist_max: "<<dist_max<<" iValue: "<<ivalue<<endl;
    //must get each node of that value    
    //while not to stopping value and still inside partition
    while(node_key <= ivalue && node_key <= dist_max && node_data != past_data)
    {
        candidates++;
        distnode = dist(&datapoints[node_data*number_dimensions], q);
        //cout<<"OUTWARD KEY: "<<node.key()<<" DATA: "<<node.data()<<" DIST: "<<distnode<<endl;
        if(distnode <= radius_r)
        {
            AddNeighbor_RESTART(node_data, distnode, restrict_num, restrict_num);    //i'm assuming you only sort when you need to
        }
        past_data = node_data;
        node++;
        node_key = node.key();
        node_data = node.data();
    }
}

//you can sort and not restrict, but you can't restrict without sort
void IDistance::AddNeighbor_RESTART(int indexID, float distnode, bool sort, bool restrict_num)
{
    //cout<<"NEIGHBOR: "<<indexID<<" Dist: "<<distnode<<endl;
    size_t k;
    
    //first we check that the data point has not already been added
    bool okay=true;
    for(int i=0;i<knn_S.size();i++)
    {
        if(knn_S.at(i) == indexID)
            okay=false;
    }
    if(okay) //if it hasn't yet...
    {
        if(!knn_S.empty() && sort) //restrict num?
        {   
            //if it needs to be inserted before last element
            if(distnode < knn_S_dists.back())
            {
                //finding the right insert spot
                for ( k = knn_S_dists.size()-1; distnode < knn_S_dists.at(k) && k > 0; k--)
                {}
                //if k=0 we need to know if we insert at 0 or 1
                if(k==0 && distnode < knn_S_dists.at(0))
                {
                    knn_S.insert(knn_S.begin(), indexID);
                    knn_S_dists.insert(knn_S_dists.begin(), distnode);
                    knn_S_nodes.insert(knn_S_nodes.begin(), stx::CountQueryNodes("Get"));
                    knn_S_candidates.insert(knn_S_candidates.begin(), candidates);
                }
                else
                {
                    //insert at one greater than k
                    knn_S.insert(knn_S.begin() + k + 1, indexID);
                    knn_S_dists.insert(knn_S_dists.begin() + k + 1, distnode);
                    knn_S_nodes.insert(knn_S_nodes.begin() + k + 1, stx::CountQueryNodes("Get"));       
                    knn_S_candidates.insert(knn_S_candidates.begin() + k + 1 , candidates);
                }
                //if we're restricting the number of elements
                if(restrict_num)
                {
                    //if we now have num+1 neighbors
                    if(knn_S.size() > knn_K)
                    {
                        knn_S.pop_back();
                        knn_S_dists.pop_back();
                        knn_S_nodes.pop_back();
                        knn_S_candidates.pop_back();
                    }
                }
            }
            else
            {
                //if we're restricting the number of elements
                if(restrict_num)
                {   //if we're still less than how many neighbors we want,
                    //we can just add it to the end
                    if(knn_S.size() < knn_K)  //class variables
                    {
                        //add to end of list
                        knn_S.push_back(indexID);
                        knn_S_dists.push_back(distnode);
                        knn_S_nodes.push_back(stx::CountQueryNodes("Get"));
                        knn_S_candidates.push_back(candidates);
                    }
                }
                else
                {
                    //add to end of list
                    knn_S.push_back(indexID);
                    knn_S_dists.push_back(distnode);
                    knn_S_nodes.push_back(stx::CountQueryNodes("Get"));
                    knn_S_candidates.push_back(candidates);
                }
            }
        }
        else
        {   //add to end of list
            knn_S.push_back(indexID);
            knn_S_dists.push_back(distnode);
            knn_S_nodes.push_back(stx::CountQueryNodes("Get"));
            knn_S_candidates.push_back(candidates);
        }
    }
}


vector<int> IDistance::KNN_SEQSEARCH(float q[], int num)
{
    knn_K = num;
    knn_S.clear();
    knn_S_dists.clear();
    knn_S_nodes.clear();
    knn_S_candidates.clear();
       
    if(num < 1)
    {
        return knn_S;
    }
    
    for(int i = 0; i < num; i++) //just fill the node counter with max
    {
        knn_S_nodes.push_back((int) theStats.treeNodes);
        knn_S_candidates.push_back((int) theStats.treeNodes);
    }
    
    int k = 0; // total k out of num saved
    float maxdist = 0.0; // max distance out of the k
    float tempdist;
    
    //prime the loop
    maxdist = dist(&datapoints[0], q);
    knn_S.push_back(0);
    knn_S_dists.push_back(maxdist);
    k = 1;
       
    //now continue on without base case checks
    for ( unsigned int i = 1; i < number_points; i++) //for each point
    {
        tempdist = dist(&datapoints[(i*number_dimensions)],q);
        //cout << "checking " << i << " : " << tempdist << " vs. " << maxdist << endl;
        if(tempdist < maxdist || k < num)
        {
            //cout << "\t keep " << i << endl;
            //we want to keep it, but need to find where it goes in sorted list
            
            int j = 0;
            vector<int>::iterator r = knn_S.begin();
            vector<float>::iterator d = knn_S_dists.begin();
            while(j < k && tempdist >= knn_S_dists[j])
            {
                d++;
                r++;
                j++;
            }
            
            //cout << "inserting at pos: " << j << endl;
            
            if(j >= k) // end of list
            {
                //cout << "j == k!!!" << endl;
                //insert at position j, and push the rest back
                knn_S_dists.push_back(tempdist);
                knn_S.push_back(i);
                maxdist = tempdist;
                //cout << "inserted at end!" << endl;
            }
            else
            {
                knn_S_dists.insert(d, tempdist);
                knn_S.insert(r, i);
                //cout << "inserted!" << endl;
            }
            
            if(k == num)
            {
                //cout << "k = num!" << endl;
                //already full, bumped one
                //drop last one
                r = knn_S.end();
                r--;
                knn_S.erase(r);
                maxdist = knn_S_dists[k-1];
                d = knn_S_dists.end();
                d--;
                knn_S_dists.erase(d);
            }
            else
            {
                //cout << "inc k " << endl;
                k++;
            }            
        }            
    }
    
    return knn_S;  
}

//*******************************************************************//
//does search with radius r from pt q and returns all results
//*******************************************************************//
vector<int> IDistance::KNN_KEEP(float q[], int num)
{
    radius_r = settings.r_init;
    knn_stopFlag = false;
    knn_K = num;
    knn_S.clear();
    knn_S_dists.clear();
    knn_S_nodes.clear();
    knn_S_candidates.clear();
    
    //variable to mark for partitions checked
    knn_partition_checked = new int[number_partitions];
    for(unsigned int i=0;i<number_partitions;i++)
        knn_partition_checked[i] = 0;
    knn_pfarthest = 0;
    //array of iterators
    knn_left_pointers = new btree_mm::iterator[number_partitions];
    knn_right_pointers = new btree_mm::iterator[number_partitions];
    
    //stx::CountQueryNodes("Start"); //reset query node count
    while(!knn_stopFlag)  //knn_ret.size() < num)//should probably have safety net (suppose num = 4, but only 3 points exist)
    {
        //increase radius (this could be a class parameter)
        radius_r += settings.r_delta;
        //check cross with each partition

        SearchO_KEEP(q, radius_r, true);
        if(radius_r > constant_c)   //assuming that constant_c is greater than max distance of space
            knn_stopFlag = true;

    }

    //look at partitions checked before finishing up
    partitionsChecked = 0;
    for(unsigned int i=0; i < number_partitions; i++)
    {
        if(knn_partition_checked[i] == 1)
        {
          partitionsChecked++;
        }
    }


    delete [] knn_partition_checked;
    delete [] knn_left_pointers;
    delete [] knn_right_pointers;
    
    return knn_S;
}


//searches partitions
void IDistance::SearchO_KEEP(float q[], float r, bool sort)
{
    //pfarthest in code is checking for the farthest neighbor in the answer
    //set S.  If |S| == #neighbors wanted and pfarthest is < r from q we
    //have all of our values
    ///////////////////set stop flag////////////////////////
    if(knn_S.size() == knn_K && knn_pfarthest <= r)
        knn_stopFlag = true;
    
    //variable to store the distance from partition to query pt
    float distp;
    //the index value of the query point
    float q_index;
    //check for each partition
    for(unsigned int i=0; i < number_partitions; i++)
    {   
        //cout<<"PARTITION: "<<i<<endl;
        
        //calc distance from query point to partition i
        distp = dist(&reference_points[i*number_dimensions],q);
        //calculate the q_index - can be moved around
        q_index = roundOff(i*constant_c + distp);
        //cout<<"Q: "<<q_index<<endl;                             
        //have we checked this partition before?
        if(knn_partition_checked[i] != 1) 
        {
            //filter dist(O_i, q) - querydist(q)="r" <= dist_max_i
            if(distp - r <= partition_dist_max[i])
            {
                knn_partition_checked[i] = 1;  //mark it as checked
                //if query sphere is inside this partition
                if(distp <= partition_dist_max[i])
                {   
                    //cout<<"Inside Partition "<<i<< ", q_index = " << q_index << endl;
                    
                    //find query pt and search in/out
                    btree_mm::iterator lnode = btree.upper_bound(q_index);
                    btree_mm::iterator rnode;// = btree.lower_bound(q_index);
                    rnode = lnode;
                    knn_left_pointers[i] = SearchInward_KEEP(lnode, q_index - r, q, i, sort);
                    knn_right_pointers[i] = SearchOutward_KEEP(rnode--, q_index + r, q, i, sort);
                }
                else //it intersects
                {
                    //cout<<"Intersect Partition "<<i<<", q_index = " << q_index <<endl;
                    //get the index value(y) of the pt of dist max
                    btree_mm::iterator dist_max_i = btree.find(datapoint_index[partition_dist_max_index[i]]);
                    knn_left_pointers[i] = SearchInward_KEEP(dist_max_i, q_index - r, q, i, sort);
                }
            }
        }
        else //we've checked it before
        {
            btree_mm::iterator null_iterator;
            if(knn_left_pointers[i] != null_iterator) //can't actually check if it's null
            {
                knn_left_pointers[i] = SearchInward_KEEP(knn_left_pointers[i]--, q_index - r, q, i, sort);
            }
            if(knn_right_pointers[i] != null_iterator)
            {
                knn_right_pointers[i] = SearchOutward_KEEP(knn_right_pointers[i]++, q_index + r, q, i, sort);
            }
        }
    }
}
//node is a pointer to the node to start from
//ivalue is the stopping value
btree_mm::iterator IDistance::SearchInward_KEEP(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool sort)
{ 
    //variable for distance
    float distnode=constant_c;
    float partition_offset = roundOff(partition_i*constant_c);
    int past_data = -1;
    //i think these will be faster
    float node_key = node.key();
    int node_data = node.data();
    //cout<<"--SearchInward--"<<endl;
    //cout<<"Part_Offset: "<<partition_offset<<" iValue: "<<ivalue<<endl;
    //cout<<"Init Node Inward: "<<node_data<<" value: "<<node_key<<endl;

    //must get each node of that value  
    //while not to stopping value and still inside partition
    
    while(node_key >= ivalue && node_key >= partition_offset && node_data != past_data)
    {        
        //candidates++;
        distnode = dist(&datapoints[node_data*number_dimensions], q);

        //cout<<"INWARD KEY: "<<node.key()<<" DATA: "<<node.data()<<" DIST: "<<distnode<<endl;
        AddNeighbor_KEEP(node_data, distnode, sort);
        past_data = node_data;
        node--;
        node_key = node.key();
        node_data = node.data();
    }
    //past partition, set node to null pointer  
    if(node_key < partition_offset || node_data == past_data) 
    {
        btree_mm::iterator node1;
        return node1;
    }
    return node;    //maybe i don't need to ++
}

btree_mm::iterator IDistance::SearchOutward_KEEP(btree_mm::iterator node, float ivalue, float q[], int partition_i, bool sort)
{
    //variable for distance
    float distnode=constant_c;
    float dist_max = roundOff(partition_i*constant_c + partition_dist_max[partition_i]);
    int past_data = -1;
    //i think these will be faster
    float node_key = node.key();
    int node_data = node.data();
    //cout<<"--SearchOutward--"<<endl;
    //cout<<"Dist_max: "<<dist_max<<" iValue: "<<ivalue<<endl;
    //cout<<"Init Node Outward: "<<node_key<<endl;

    //must get each node of that value    
    //while not to stopping value and still inside partition
    while(node_key <= ivalue && node_key <= dist_max && node_data != past_data)
    {        
        //candidates++;
        distnode = dist(&datapoints[node_data*number_dimensions], q);
        //cout<<"OUTWARD KEY: "<<node.key()<<" DATA: "<<node.data()<<" DIST: "<<distnode<<endl;
        AddNeighbor_KEEP(node_data, distnode, sort);

        past_data = node_data;
        node++;
        node_key = node.key();
        node_data = node.data();
    }
    //past partition, set node to null pointer  
    if(node_key > dist_max || node_data == past_data) 
    {
        btree_mm::iterator node1;
        return node1;
    }
    return node;    //maybe i don't need to --
}


//you can sort and not restrict, but you can't restrict without sort
void IDistance::AddNeighbor_KEEP(int indexID, float distnode, bool sort)
{
    //cout<<"NEIGHBOR: "<<indexID<<" Dist: "<<distnode<<endl;
    size_t k;
    bool okay=true;
    for(int i=0;i<knn_S.size();i++)
    {
        if(knn_S.at(i) == indexID)
            okay=false;
    }
    if(okay) //if it hasn't yet...
    {
        candidates++;
        if(!knn_S.empty() && sort) //restrict num?
        {   
            //if it needs to be inserted before last element
            if(distnode < knn_S_dists.back())
            {
                //finding the right insert spot
                for(k=knn_S_dists.size()-1; distnode < knn_S_dists.at(k) && k>0; k--)
                {}
                //if k=0 we need to know if we insert at 0 or 1
                if(k==0 && distnode < knn_S_dists.at(0))
                {
                    knn_S.insert(knn_S.begin(), indexID);
                    knn_S_dists.insert(knn_S_dists.begin(), distnode);
                    knn_S_nodes.insert(knn_S_nodes.begin(), stx::CountQueryNodes("Get"));
                    knn_S_candidates.insert(knn_S_candidates.begin(), getCandidateCount());
                }
                else
                {
                    //insert at one greater than k
                    knn_S.insert(knn_S.begin() + k + 1, indexID);
                    knn_S_dists.insert(knn_S_dists.begin() + k + 1, distnode);
                    knn_S_nodes.insert(knn_S_nodes.begin() + k + 1, stx::CountQueryNodes("Get"));               
                    knn_S_candidates.insert(knn_S_candidates.begin() + k + 1, getCandidateCount());
                }
            }
            else
            {
                //if we're still less than how many neighbors we want,
                //we can just add it to the end
                if(knn_S.size() < knn_K)  //class variables
                {
                    //add to end of list
                    knn_S.push_back(indexID);
                    knn_S_dists.push_back(distnode);
                    knn_S_nodes.push_back(stx::CountQueryNodes("Get"));
                    knn_S_candidates.push_back(getCandidateCount());
                }
                else
                {
                    //add to end of list
                    knn_S.push_back(indexID);
                    knn_S_dists.push_back(distnode);
                    knn_S_nodes.push_back(stx::CountQueryNodes("Get"));
                    knn_S_candidates.push_back(getCandidateCount());
                }
            }
            
            
            if(knn_S.size() > knn_K)  //could also change to while
            {   
                knn_S.pop_back();
                knn_S_dists.pop_back();
                knn_S_nodes.pop_back();
                knn_S_candidates.pop_back();
                knn_pfarthest = knn_S_dists[knn_K-1];
            }
            else
            {
                knn_pfarthest = knn_S_dists.back();
            }
        }
        else
        {   
            if(distnode <= radius_r || sort) //or sort???
            {
                //add to end of list
                knn_S.push_back(indexID);
                knn_S_dists.push_back(distnode);
                knn_S_nodes.push_back(stx::CountQueryNodes("Get"));
                knn_S_candidates.push_back(getCandidateCount());
                
          
                if(distnode > knn_pfarthest)
                    knn_pfarthest = distnode;
            }
        }
    }
}


//*******************************************************************//
//*******************************************************************//
void IDistance::setData( std::vector<unsigned int>& ids, std::vector<float>& data, int n, int d)
{
	datapoint_ids = ids;
	datapoints = data;
	number_points = n;
    number_dimensions = d;
}

/******************************
 * These options have already been established by the config file
 * but some require knowing information about the data. this function
 * is called after data is set.
 ************************************/ 
void IDistance::initOptions()
{
  
  if(settings.c_type == CALCULATE)
  {
    constant_c = (float) calculateC(number_dimensions, settings.refs_dist);
  }
  else if(settings.c_type == MANUAL)
  {
    constant_c = settings.c_val;
  }
  
  if(settings.refs_build != REF_FILE)
  {
    buildReferencePoints();
  }
  
  
  if(settings.algo_version == EXT1)
  {
	  if(settings.splits_build != SPLIT_FILE)
	  {
		buildSplits();
	  }
  }
}

void IDistance::setSequentialSearch(int s)
{
    doSeqSearch = s;
}

int IDistance::getNumberOfPoints()
{
    return number_points;
}

int IDistance::getNumberOfDimensions()
{
    return number_dimensions;
}

//*******************************************************************//
//This function takes the list of reference points and uses them to
//to assign a partition to all of the data points.  Then it calculates
//the index value and stores it in an array.
//*******************************************************************//
void IDistance::buildIndexValues()
{
  
    switch(settings.refs_assign)
    {
      case CLOSEST:
        buildIndex_CLOSEST();
        break;
      case ASSIGN_FILE:
        buildIndex_ASSIGN();
        break;
    }
    
    
}

void IDistance::buildIndex_CLOSEST()
{
    
    //initialize the arrays
    datapoint_index = new float[number_points];
    partition_dist_max = new float[number_partitions];
    partition_dist_max_index = new unsigned int[number_partitions];
    for (unsigned int k=0; k < number_partitions; k++) {
        partition_dist_max[k] = 0.0;    // Initialize all elements to zero.
        partition_dist_max_index[k] = -1; // if -1, partition contains NO points!
    }
    
    
    
    float ref_dist, temp_dist;
    int current_partition=0;
    int offset;
    
    for(unsigned int i=0 ; i < number_points ; i++) //for each point
    {
        
        //cout << "Point " << i << ": " << endl;
        
        //use idp for inspecting data points
        //bool idp = false;
        //if(datapoint_ids[i] == 700 || datapoint_ids[i] == 872 || datapoint_ids[i] == 927) { idp = true; }
        
        offset = i*number_dimensions;   //loop invariant
        
        current_partition = 0;
        ref_dist = dist(&datapoints[offset],&reference_points[0]);
        
        //if(idp) { cout << "checking point " << i << endl; }
        
        //if(idp) { cout << "p 0: " << ref_dist << endl; }
         
        //check each partition (baseline from partition 0)
        for(unsigned int part=1; part < number_partitions; part++)
        {
            //get distance to this partition
            temp_dist = dist(&datapoints[offset],&reference_points[part*number_dimensions]);
        
            //if(idp) { cout << "p " << part << ": " << temp_dist << endl; }
            
            if(temp_dist < ref_dist) //if closer to this partition
            {
                //if(idp) { cout << "  - updating from " << current_partition << endl; }
                
                current_partition = part; //update
                ref_dist = temp_dist;
                
            }
        }
        
        
        //if(idp) { cout << "final part = " << current_partition << " : " << ref_dist << endl; }
        
        //calculate key index value
        //datapoint_index[i] = current_partition*constant_c + ref_dist;
        
        //updated to try to fix float precision issues
        temp_dist = roundOff(current_partition*constant_c + ref_dist);
        
        //if(idp) { cout << "  index_dist = " << temp_dist << endl; }
        
        datapoint_index[i] = temp_dist;
        
        //cout << "  DATA INDEX: " << datapoint_index[i] << " P: " <<
        //  current_partition << " D: " << ref_dist << endl;
        
        //also need to update partition dist_max
        if(ref_dist > partition_dist_max[current_partition])
        {
            partition_dist_max[current_partition] = ref_dist;
            partition_dist_max_index[current_partition] = i;
        }
    }
}


void IDistance::buildIndex_ASSIGN()
{
    //given that we already have the assignments loaded up from file
    if(point_assignments == NULL)
    {
      cout << "No assignment file loaded up!" << endl;
      return;
    }
    
    //initialize the arrays
    datapoint_index = new float[number_points];
    partition_dist_max = new float[number_partitions];
    partition_dist_max_index = new unsigned int[number_partitions];
    for (unsigned int k=0; k < number_partitions; k++) {
        partition_dist_max[k] = 0.0;    // Initialize all elements to zero.
        partition_dist_max_index[k] = -1; // if -1, partition contains NO points!
    }
    
    
    float ref_dist, temp_dist;
    int part, offset;
    
    for(unsigned int i=0 ; i < number_points ; i++) //for each point
    {
        
        cout << "  Point " << i << " assigned to: " << point_assignments[i] << endl;
        
        offset = i*number_dimensions;   //loop invariant
        
        part = (int) point_assignments[i];
        ref_dist = dist(&datapoints[offset],&reference_points[part*number_dimensions]);

        //updated to try to fix float precision issues
        temp_dist = roundOff(part*constant_c + ref_dist);
        
        //if(idp) { cout << "  index_dist = " << temp_dist << endl; }
        
        datapoint_index[i] = temp_dist;
        
        //cout << "  DATA INDEX: " << datapoint_index[i] << " P: " <<
        //  current_partition << " D: " << ref_dist << endl;
        
        //also need to update partition dist_max
        if(ref_dist > partition_dist_max[part])
        {
            partition_dist_max[part] = ref_dist;
            partition_dist_max_index[part] = i;
        }
        
        
    }
}

//*******************************************************************//
// This outputs the reference points in order of partition.
//*******************************************************************//
void IDistance::outputReferencePoints()
{
  int offset;
  for(unsigned int i=0; i < number_partitions; i++) //for each partition
  {
      cout << "Partition " << i << ": ";
      offset = i*number_dimensions; //partition offset into 1D array
      for(unsigned int j=0; j < number_dimensions; j++) //for each dimension
      {
          if(j != 0)
          {
            cout << ", ";
          }
          cout << reference_points[offset+j];
      }
      cout << endl;
  }
  
  
}


void IDistance::buildSplits()
{
    switch(settings.refs_build)
    {
      case HALF_POINTS:
        buildSplits_TOP_CENTERS();
        break;
      case REF_FILE:
        //already done before we init to here
        break;
    }
	
}

void IDistance::buildSplits_TOP_CENTERS()
{
	
	//use findmedians (copied below from Helper class)
	//pick top X splits near the center of the data space (0.5)

	
	float* medianVals = findMedians(&datapoints[0], number_points, number_dimensions, false);
	
	cout << "MEDIANS: " << endl;
	for(unsigned int i = 0; i < number_dimensions; i++)
	{
		cout << medianVals[i] << "," << endl;
	}
	
}




/*****************************************************
 * Brute force method to find the median of each
 * dimension and return them as an array. Note the medians
 * are in the same order as the original point dimensions.
 * If the data still has an ID attribute (as the first dim)
 * then the input flag will remove it and return just
 * the (d-1) median values
 * 
 * Input: data, number of points, number of dimensions, Id dimension flag
 * Output: array of median values as floats
 */ 
 float* IDistance::findMedians(float* data, int p, int d, bool hasID)
{
  
  float* meds = new float[d]; //found median array
  
  //set up for odd number of points
  bool medSplit = false;  
  int m = (p / 2);
  int m2 = -1; //not used for odd
  
  if(p % 2 == 0) //if even number of points
  {
    medSplit = true;
    m = (p / 2) - 1;
    m2 = (p / 2);
  }
  
//  cout << "m | m2 = " << m << " | " << m2 << endl;

  int i = 0;
  if(hasID)
  {
    i = 1; //skip first dimension if its the ID attribute
  }
  while(i < d)
  {
    //float* points = new float[p];
    vector<float> points;
    
//    cout << i << ": ";
    
    for(int j = 0; j < p; j++)
    {
      //add each point's ith value to the vector
      points.push_back(  data[(j*d) + i] );
//      cout << points[j] << ",";
    }
//    cout << endl;
    
    //sort points vector
    sort(points.begin(), points.end());
    
    //get and save median
//    cout << "sorted: ";
//    for(int j = 0; j < p; j++)
//    {
//      cout << points[j] << ",";
//    }
//    cout << endl;
  
    if(medSplit)
    {
      //average of the two middle points
      meds[i] = (points[m] + points[m2]) / 2.0f;
    }
    else
    {
      meds[i] = points[m];
    }
    
    i++; //go to next dimension
  }
  
  return meds;
}



//*******************************************************************//
// Master Reference point building function which calls the specialized
// methods. These methds have the common naming convention
// "buildRefs_" + enum name. 
// Also note, FROM_FILE build method occurs before this because of the file load
//*******************************************************************//
void IDistance::buildReferencePoints()
{
    //cout << "building with refs: " << settings.refs_build << endl;
    switch(settings.refs_build)
    {
      case HALF_POINTS:
        buildRefs_HALF_POINTS();
        break;
      case HALF_POINTS_CENTER:
        buildRefs_HALF_POINTS_CENTER();
        break;
      case HALF_POINTS_OUTSIDE:
        //cout << "HERE!!!" << endl;
        buildRefs_HALF_POINTS_OUTSIDE();
        break;
      case RANDOM:
        buildRefs_RANDOM();
        break;
      case CLUSTER:
        buildRefs_CLUSTER();
        break;
      case UNIFORM:
        buildRefs_UNIFORM();
        break;
      case REF_FILE:
        //already done.
        break;
    }

}



void IDistance::buildRefs_HALF_POINTS()
{
  //for every dimension i, two points:
  //(0.5, 0.5, ..., i = 0.0, ..., 0.5)
  //(0.5, 0.5, ..., i = 1.0, ..., 0.5)
  
  number_partitions = number_dimensions*2;
  reference_points = new float[number_partitions*number_dimensions];
  
  //initialize everything to 0.5
  for(unsigned int i = 0; i < number_partitions * number_dimensions; i++)
  {
    reference_points[i] = 0.5;
  }
   
  //set 0, 1 values
  //half_offset is to the start of the second half of ref points
  //the dim^2 is because i want dim*(.5*partss) and parts = 2*dim
  int half_offset = number_dimensions*number_dimensions;
  for(unsigned int i=0; i < number_dimensions; i++) //for each dimension
  {
      //set only its points to 0 and 1, 
      //walking diagonally through array in both spots at once
      reference_points[(i*number_dimensions) + i] = 0;
      reference_points[(half_offset) + (i*number_dimensions) + i] = 1;
  }

}

void IDistance::buildRefs_HALF_POINTS_CENTER()
{
  
    //plus one more ref point in the space center,
    //so this point remains all 0.5 values.
    number_partitions = number_dimensions*2 + 1;
    reference_points = new float[number_partitions*number_dimensions];
    
    //initialize everything to 0.5
    for(unsigned int i = 0; i < number_partitions * number_dimensions; i++)
    {
      reference_points[i] = 0.5;
    }
    
    //set 0, 1 values
    //half_offset is to the start of the second half of ref points
    //the dim^2 is because i want dim*(.5*partss) and parts = 2*dim
    int half_offset = number_dimensions*number_dimensions;
    for(unsigned int i=0; i < number_dimensions; i++)
    {
        //set only its points to 0 and 1, 
        //walking diagonally through array in both spots at once
        reference_points[i*number_dimensions + i] = 0;
        reference_points[(half_offset) + (i*number_dimensions) + i] = 1;
    }
    //it will leave the last partition alone with all .5s.

}


void IDistance::buildRefs_HALF_POINTS_OUTSIDE()
{
  //for every dimension i, two points:
  //(0.5, 0.5, ..., i = MIN, ..., 0.5)
  //(0.5, 0.5, ..., i = MAX, ..., 0.5)
  
  //where MIN and MAX are the arbitrarily far outside points from the dataspace
  // example, lets set them to -10, +11
  
  
//  cout << "HERE! outside" << endl;
  
  int outside_min = 0 - settings.refs_dist;
  int outside_max = 1 + settings.refs_dist;
  
  number_partitions = number_dimensions*2;
  reference_points = new float[number_partitions*number_dimensions];
  
  //initialize everything to 0.5
  for(unsigned int i = 0; i < number_partitions * number_dimensions; i++)
  {
    reference_points[i] = 0.5;
  }
   
  //set 0, 1 values
  //half_offset is to the start of the second half of ref points
  //the dim^2 is because i want dim*(.5*partss) and parts = 2*dim
  int half_offset = number_dimensions*number_dimensions;
  for(unsigned int i=0; i < number_dimensions; i++) //for each dimension
  {
      //set only its points to 0 and 1, 
      //walking diagonally through array in both spots at once
      reference_points[(i*number_dimensions) + i] = (float) outside_min;
      reference_points[(half_offset) + (i*number_dimensions) + i] = (float) outside_max;
  }

}


void IDistance::buildRefs_RANDOM()
{
  
  number_partitions = number_dimensions*2;
  reference_points = new float[number_partitions*number_dimensions];

  //cout.precision(10);
  
  for(unsigned int i = 0; i < number_partitions * number_dimensions; i++)
  {
    float temp = (float) rand();
    temp = temp / 100000.0f;
    temp = roundOff(temp - int(temp));
    //cout << "temp3 = " << temp << endl;

    reference_points[i] = temp;
  }
  
}


void IDistance::buildRefs_CLUSTER()
{
    
    
}

void IDistance::buildRefs_UNIFORM()
{
  
  //DOES NOT WORK YET!!
  number_partitions = number_dimensions*2;
  reference_points = new float[number_partitions*number_dimensions];

  //cout.precision(10);
  
  int splits_per_dim = number_partitions / number_dimensions;
  float split_length = 1.0f / (splits_per_dim + 1.0f);
  
  cout << "splits per dimension = " << endl;
  for(int i = 1; i <= splits_per_dim; i++)
  {
    cout << split_length * i << endl;
  }
  
  for(unsigned int i = 0; i < number_partitions; i++)
  {
      float temp = 0.0;

      reference_points[i] = temp;  
  }
    
}


//*******************************************************************//
// if given the split points: splits 1D array, num is the number of splits
// first line is the dims indexes
// second line is the split vals for each dim
//*******************************************************************//
void IDistance::setSplitPoints(float* splits, int num)
{
    //cout << "HERE!" << endl;
    
	for(int i = 0; i < num; i++)
	{
		cout << "split " << i << " on dim " << splits[i] 
			 << " at val " << splits[num + i] << endl; 
	}
	
	
}


//*******************************************************************//
// if given the reference points
//*******************************************************************//
void IDistance::setReferencePoints(float* refs, int num)
{
    reference_points = refs;
    number_partitions = num;
}


void IDistance::setPointAssignment(float* assigns, int num)
{
  point_assignments = assigns;
  
}

//*******************************************************************//
// This function inserts all of the ids into the b+tree.  This is
// simply so that we can time how long it takes.
//*******************************************************************//
void IDistance::buildTree()
{
    for(unsigned int i = 0; i < number_points; i++)
    {
        //cout << i << "=" << datapoint_index[i]<<endl;
        
        btree.insert2(datapoint_index[i],i);
    }
        
     //___________STAT COLLECTION___________//
	btree_mm :: tree_stats ts;
	ts = btree.get_stats();
	theStats.treeItems = ts.itemcount;
	theStats.treeNodes = ts.nodes(); //fulfills node count
	theStats.treeLeaves = ts.leaves;
	theStats.treeInner = ts.innernodes;
	theStats.treeLevels = ts.levels;
	theStats.treeAver = ts.avgfill_leaves();

	//ofstream fragFile;
	//fragFile.open ("treeFrag.txt");
	//btree.print_tree_segments(fragFile);
	//fragFile.close();
     //___________STAT COLLECTION___________//
}

//*******************************************************************//
// writes tree to serialized file
// 
// Takes in a filename without extension
// and writes out .tree and .data binary files. The .tree is a serialized
// output included with the standard stx b+ tree. The .data file contains
// a sequence of important data required in addition to the tree data
//*******************************************************************//
void IDistance::saveTree(const char* fStr)
{
    char* ext= new char[5];
    strcpy(ext, ".tree");
    char* fname = new char[strlen(fStr)+6]; 
    strcpy(fname, fStr);
    
    ofstream fout;
    fout.open(strcat(fname,ext), ios::out);
    btree.dump(fout);
    fout.close();
    
    //save other data structures
    strcpy(ext, ".data");
    strcpy(fname, fStr);
    
    fout.open(strcat(fname,ext), ios::out | ios::binary);
    
    //write needed metadata
    fout.write((char *)(&number_points), sizeof(number_points));
    fout.write((char *)(&number_dimensions), sizeof(number_dimensions));
    fout.write((char *)(&number_partitions), sizeof(number_partitions));
    fout.write((char *)(&constant_c), sizeof(constant_c));
    //write datapoints
    fout.write(reinterpret_cast<const char*> (&datapoints[0]), sizeof(float)*number_points*number_dimensions);
    //write reference points
    fout.write(reinterpret_cast<const char*> (reference_points), sizeof(float)*number_partitions*number_dimensions);
    //write partition dist max
    fout.write(reinterpret_cast<const char*> (partition_dist_max), sizeof(float)*number_partitions);
    //write partition_dist_max_index
    fout.write(reinterpret_cast<const char*> (partition_dist_max_index), sizeof(int)*number_partitions);
    //write datapoint ids
    fout.write(reinterpret_cast<const char*> (&datapoint_ids[0]), sizeof(float)*number_points);
    //write datapoint index
    fout.write(reinterpret_cast<const char*> (&datapoint_index[0]), sizeof(float)*number_points);
    
    fout.close();
}

//*******************************************************************//
// loads tree from serialized file
//
// this takes in a .tree file name and reads in the
// binary tree file and associated .data files with the same name.
//*******************************************************************//
void IDistance::loadTree(const char* fp)
{
    
    string dataStr = fp;
    dataStr = dataStr.substr(0, strlen(fp)-5);
    dataStr = dataStr + ".data";
    
    ifstream fin;
    fin.open(fp, ios::in);
    btree.restore(fin);
    fin.close();
    
    //load other data
    fin.open(dataStr.c_str(), ios::in | ios::binary);
    
    //read needed metadata
    fin.read((char *)(&number_points), sizeof(number_points));
    fin.read((char *)(&number_dimensions), sizeof(number_dimensions));
    fin.read((char *)(&number_partitions), sizeof(number_partitions));
    fin.read((char *)(&constant_c), sizeof(constant_c));
    
    
    //read datapoints
    datapoints.resize(number_points*number_dimensions);
    fin.read(reinterpret_cast<char *> (&datapoints[0]), sizeof(float)*number_points*number_dimensions);
    //read reference points
    reference_points = new float[number_partitions*number_dimensions];
    fin.read(reinterpret_cast<char *> (reference_points), sizeof(float)*number_partitions*number_dimensions);
    //read partition dist max
    partition_dist_max = new float[number_partitions];
    fin.read(reinterpret_cast<char *> (partition_dist_max), sizeof(float)*number_partitions);
    //read partition_dist_max_index
    partition_dist_max_index = new unsigned int[number_partitions];
    fin.read(reinterpret_cast<char *> (partition_dist_max_index), sizeof(int)*number_partitions);
    //read datapoint ids
    datapoint_ids.resize(number_points);
    fin.read(reinterpret_cast<char *> (&datapoint_ids[0]), sizeof(float)*number_points);
    //read datapoint index
    datapoint_index = new float[number_points];
    fin.read(reinterpret_cast<char *> (datapoint_index), sizeof(float)*number_points);
    
    fin.close();
}
    
//*******************************************************************//
//Euclidean distance function.
//*******************************************************************//
float IDistance::dist(float p1[], float p2[])
{
    float sum = 0.0;
    float current_dist, diff;
    
    for(unsigned int j = 0; j < number_dimensions; j++) //for each dimension
    {
        diff = p1[j] - p2[j]; //subtract
        sum += diff*diff; //and square, keep the total sum
        
        //cout<<" DIFF: "<<diff;
        //cout<<" SUM: "<<sum<<endl;
    }
    
    current_dist = roundOff(sqrt(sum));
    
    //cout<<"DIST ROOT: "<<current_dist<<endl;
    return current_dist;
}
//*******************************************************************//
// options are set via a struct with the possibilities
//*******************************************************************//
void IDistance::setOptions(configOptions co)
{
    settings = co;
}

//*******************************************************************//
// Given a vector of indices, return a vector of point IDs
//*******************************************************************//
vector<int> IDistance::getIDs(vector<int> indices)
{
    vector<int> ret;
    for(int i=0;i<indices.size();i++)
    {
        ret.push_back(datapoint_ids[indices[i]]);
    }
    return ret;
}


void IDistance::resetCandidateCount()
{
  
  candidates = 0;
  
}

int IDistance::getCandidateCount()
{
  if(doSeqSearch == 1) //do sequential search instead!
  {
      return number_points; // return list of final set
  }
  return candidates;
  
}



void IDistance::resetPartitionCount()
{
  
  partitionsChecked = 0;
  
}

int IDistance::getPartitionCount()
{
  if(doSeqSearch == 1) //do sequential search instead!
  {
      return number_partitions; // return list of final set
  }
  return partitionsChecked;
  
}
//*******************************************************************//
// Return the vector of knn distances 
//*******************************************************************//
vector<float> IDistance::getKNN_dists()
{
    return knn_S_dists;
}
//*******************************************************************//
// Return the vector of knn num nodes visited
//*******************************************************************//
vector <int> IDistance::getKNN_nodes()
{
	return knn_S_nodes;
}

vector <int> IDistance::getKNN_candidates()
{
	return knn_S_candidates;
}

float IDistance::getKNN_avgDist()
{
	return AverageDist(knn_S_dists);
}

//*******************************************************************//
// Externally reset the node visit counter
//*******************************************************************//
void IDistance::resetNodeCount()
{
  stx::CountQueryNodes("Start");
}

int IDistance::getNodeCount()
{
  if(doSeqSearch == 1) //do sequential search instead!
  {
      return (int) theStats.treeNodes; // return total nodes in tree
  }
  
  return stx::CountQueryNodes("Get");
}

//*******************************************************************//
// Prints the query statistics, including
// Query answers, the number of nodes visited for each node in the answer, 
// the distance from the query point to the nodes, the average of those 
// distances, and the total time in milliseconds.   
//*******************************************************************//
/*
void iDistance::PrintQueryStats(ostream &out)
{
	//check to make sure there are results to match
     if(theStats.ind.empty())
	out <<"There are no matches for this query." << endl;
     else{
	    out<<"  RESULTS:"<<endl;
	    out << "    ";
	    printVector(getIDs(theStats.ind), out, true);

	    out<<"  Nodes Visited: " <<endl;
	    out << "    ";
	    printVector(theStats.numNodes, out, true);

	    if (theStats.knn)
	    {
		out<<"  kNN Distances: " <<endl;
		printVector(knn_S_dists, out, false);
	    
		out<<"  kNN Average Distance: " << AverageDist(knn_S_dists) << endl;
	    }
      }
    out<<"  Query Search Time: " << theStats.queryTime << " Milliseconds" <<endl;
    out<<endl;
}
*/
//*******************************************************************//
/*
void iDistance::PrintQueryStats(const char* fp)
{
    char* ext= new char[8];
    strcpy(ext, ".results");
    char* fname = new char[strlen(fp)+9]; 
    strcpy(fname, fp);
    
    //Print to File 
    ofstream resFile;
    resFile.open(strcat(fname,ext));
    if ( !resFile.is_open() )
    {
      cout << "Unable to open file!" << endl;
      return;
    }
    PrintQueryStats(resFile);
    resFile.flush();
    resFile.close();   
}

*/

//*******************************************************************//
void IDistance::getStats(stats* s)
{
  *s = theStats;
}

void IDistance::PrintNodeRange(const char* fp)
{
  //Print to File 
    char* ext= new char[9];
    strcpy(ext, ".segments");
    char* fname = new char[strlen(fp)+10]; 
    strcpy(fname, fp);
 
    ofstream segFile;
    segFile.open(strcat(fname,ext));
    if( !segFile.is_open() )
    {
      cout << "Unable to open file!" << endl;
      return;
    }
    
    PrintNodeRange(segFile);
    
    segFile.flush();
    segFile.close();
    
}

//*******************************************************************//
//print node ranges
//*******************************************************************//
void IDistance::PrintNodeRange(ostream &out)
{
    int leaves = (int) theStats.treeLeaves;
//    out<<"Number of leaves: "<<leaves<<endl;
    //print the ranges in each node
    float low,high;
    //put iterator at very first key entry
    btree_mm::iterator node=btree.lower_bound(0.0);
    //nodecount starts at 0;
    int nodecount=stx::CountQueryNodes("Start");
    
    low = node.key();
    while(nodecount<leaves-1)   
    {
       //we've moved to a new node
       if(nodecount < stx::CountQueryNodes("Get"))
       {
 //          out<<"Node "<<nodecount+1<<" Low: "<<low<<" High: "<<high<<endl;
           out << low << " " << high << endl;
 
           low = node.key();
           high = 0;
           //nodecount = stx::CountQueryNodes("Get");
           nodecount++;
       }
       else
       {
           high = node.key();
           node++;       
       }
    }
    
    float old_high = -1;
    while(high != old_high)
    {
        old_high = high;
        node++;
        if(node.key() > 0)
            high=node.key();
    }
 //   out<<"Node "<<nodecount+1<<" Low: "<<low<<" High: "<<high<<endl;
    out << low << " " << high << endl;
    out.flush();
}


//*******************************************************************//
// Return the average distance of a vector
//   input: a vector of floats 
//   output: the average of the input
//*******************************************************************//
float IDistance::AverageDist(vector<float>& distances)
{
	float aver = 0.0;	
	for (int i = 0; i < distances.size(); i++){
		aver = aver + distances[i];
	}
	aver = aver / distances.size();

	return roundOff(aver);
}

//*******************************************************************//
//Print the elements of a vector 
//  input: Vector vec to be printed
//         Boolean commas, true -separate elements with a comma
//*******************************************************************//
template<typename T>
void IDistance::printVector(const std::vector<T>& vec, ostream &out, bool commas)
{
    if(commas)
    {
        for (int i = 0; i < vec.size()-1; i++)
        {
            out<< vec[i] << ", ";
        }
        out<<vec[vec.size()-1]<<endl;
    }
    else
    {
        for (int i = 0; i < vec.size(); i++)
        {
            out<< "    " << vec[i] << endl;
        }
    }
}

/***************************************************
 * Calculates the theoretical upper boundary distance of two points
 * in d dimensions of [0,1] space. This can be set as an non-optimal
 * c value that will gaurantee separation in the b+ tree.
 ****************************************************/
int IDistance::calculateC(int d, int e)
{
  
  //cout << "calculate c given " << d << ",  " << e << endl;
  
  //added so we can dynamically set c based on what config file refs-dist is
  //space given extra e (default is e=0, no affect)
  float edist = (float) ( (2 * e) + 1 );
  
  //since each dim is max length of 1
  //multiply by 2 for safety 
  float diag = 2 * sqrt( (edist * d));
  
  //old
//  float diag = sqrt(d);
  
  //added ONLY for Tim's EM algorithm 4/25/2012
  //diag = diag * 3;
  

  //round up
  int c = (int)(diag + 0.5);

  //cout << "final c = " << c << endl;
  
  return c;

}


float IDistance::roundOff(float in)
{
    int temp = (int)((in * 1000.0f) + 0.5f);
    return (temp / 1000.0f);
}


float IDistance::calcIndex(int p, float c, float d)
{
    return roundOff( (p*c + d) );
}

//*******************************************************************//
//this does the knn search.
//*******************************************************************//


