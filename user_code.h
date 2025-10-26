#ifndef USER_CODE_H
#define USER_CODE_H

// Feel free to include more library functions
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <climits>
#include <bits/stdc++.h>
using namespace std;


/* 
Complete this function
*/

vector<vector<int>> question_one(const vector<vector<int>>& parcels) {
    // TODO: Implement function
    unordered_map<int, pair<int, int>> mp; // parcel_id -> (min_weight, max_weight)

    //Go through each parcel and update min and max weights 
    for (const auto& parcel : parcels) {
        int pid = parcel[0];
        int w = parcel[1];

        if (mp.find(pid) == mp.end()) {
            mp[pid] = {w, w}; // initialize min and max
        } else {
            mp[pid].first = min(mp[pid].first, w); // update min
            mp[pid].second = max(mp[pid].second, w); // update max
        }
    }

    // Go through each item in mp if min and max weights are different
    // add to output
    vector<vector<int>> output;
    for (const auto& [pid, weights] : mp) {
        if (weights.first != weights.second) { // only if different weights exist
            output.push_back({pid, weights.first});
        }
    }

    // Sort output by parcel id
    sort(output.begin(), output.end());

    return output;
}

/* 
Complete this function
*/

// given preorder and inorder traversals of a binary tree, reconstruct the tree 
// with node { int val; TreeNode *left; TreeNode *right;};

class TreeNode {
public:
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

// Assign parcels to leaf nodes from leafParcels in order given in input

//buildTreeHelper function to build tree from preorder and inorder and stores leaf ids in leaflist
//Time complexity : O(N ) where N is number of nodes in tree
//Space complexity : O(N) for recursion stack and hashmap


TreeNode* buildTreeHelper(const vector<int>& preorder, int preStart, int preEnd, const vector<int>& inorder, int inStart, int inEnd, unordered_map<int, int>& inMap, vector<int>& leaflist) 
{
    if (preStart > preEnd || inStart > inEnd) return nullptr;

    int rootVal = preorder[preStart];
    TreeNode* root = new TreeNode(rootVal);

    int inRoot = inMap[rootVal];
    int numsLeft = inRoot - inStart;

    root->left = buildTreeHelper(preorder, preStart + 1, preStart + numsLeft,
                                 inorder, inStart, inRoot - 1, inMap, leaflist);

    root->right = buildTreeHelper(preorder, preStart + numsLeft + 1, preEnd,
                                  inorder, inRoot + 1, inEnd, inMap, leaflist);

    //if leaf node
    if (!root->left && !root->right) {
        leaflist.push_back(root->val);
    }

    return root;
}

//storePaths function that stores paths from root to each leaf node in a hashmap
//Time complexity : O(N) where N is number of nodes in tree
//Space complexity : O(N) for recursion stack and hashmap


// fucntion that takes root and hashmap to store path from root to leaf node at each leaf node using backtracking
void storePaths(TreeNode* root, unordered_map<int, vector<int>>& leafPaths, vector<int>& currentPath) {
    if (!root) return;

    // Add current node to path
    currentPath.push_back(root->val);

    // If leaf node, store the path
    if (!root->left && !root->right) {
        // Store a copy of currentPath
        leafPaths[root->val] = currentPath;
    } 
    else 
    {
        storePaths(root->left, leafPaths, currentPath);
        storePaths(root->right, leafPaths, currentPath);
    }

    // Backtrack - remove current node from path
    currentPath.pop_back();
}

//findHighestJunction function that finds the highest junction node for a given query
//Time complexity : O(P * logH + P) where P is number of parcels in each query and H is height of tree
//Space complexity : O(H) for recursion stack and hashmap



// Function to find the highest junction node for given root, parcelToLeaf map, leafPaths map and a query
//LOGIC : Main idea is to use binary search on the paths from root to each leaf node since it will always be increasing sequence.
//corner case  : if all parcels in query belong to same leaf node, return that leaf node value.
int findHighestJunction(TreeNode* root, unordered_map<int, int>& parcelToLeaf, unordered_map<int, vector<int>>& leafPaths, const vector<int>& query) {
    if (!root) return -1;

    // If query is empty, return -1
    if (query.empty()) return -1;

    // Find the leaf node for the first parcel in query
    int firstParcel = query[0];
    int leafNodeVal = parcelToLeaf[firstParcel];

    //store leafnodeVal of parcel for which path lenght is minimum
    //This is an optimization to reduce the path length to be searched
    int minPathLen = leafPaths[leafNodeVal].size();
    int minLeafNodeVal = leafNodeVal;

    // Check if all parcels in query belong to the same leaf node
    bool allSameLeaf = true;
    for (int parcel : query) {
        //Assuming all parcels of query are always valid and exist in parcelToLeaf map
        if (parcelToLeaf[parcel] != leafNodeVal) {
            allSameLeaf = false;
        }

        if (leafPaths[parcelToLeaf[parcel]].size() < minPathLen) {
            minPathLen = leafPaths[parcelToLeaf[parcel]].size();
            minLeafNodeVal = parcelToLeaf[parcel];
        }
    }

    if (allSameLeaf) {
        //Best case optimization
        return leafNodeVal; // all parcels belong to same leaf node
    }

    // Get the path from root to this leaf node
    const vector<int>& path = leafPaths[minLeafNodeVal];
 
    int left = 0;
    int right = path.size() - 1;
    int highestJunction = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int currentNodeVal = path[mid];

        // Check if all parcels in query are present in the subtree rooted at currentNodeVal
        bool allPresent = true;
        for (int parcel : query) {

            // Assuming all parcels of query are always valid and exist in parcelToLeaf map
            // Uncomment below lines if you want to handle invalid parcels in query
            // if (parcelToLeaf.find(parcel) == parcelToLeaf.end()) {
            //     allPresent = false;
            //     break; // parcel not found
            // }

            int parcelLeafVal = parcelToLeaf[parcel];
            const vector<int>& parcelPath = leafPaths[parcelLeafVal];

            // Use binary search to check if currentNodeVal is in the path of this parcel
            if (!binary_search(parcelPath.begin(), parcelPath.end(), currentNodeVal)) {
                allPresent = false;
                break;
            }
        }

        if (allPresent) {
            highestJunction = currentNodeVal; // update highest junction
            left = mid + 1; // try for a higher junction
        } else {
            right = mid - 1; // go lower
        }
    }   

    return highestJunction;
}


vector<int> question_two(
    const vector<int>& preorder,
    const vector<int>& inorder,
    const vector<vector<int>>& leafParcels,
    const vector<vector<int>>& query
) {
    // TODO: Implement function

    // Reconstruct the tree from preorder and inorder
    unordered_map<int, int> inMap;
    for (int i = 0; i < inorder.size(); ++i) {
        inMap[inorder[i]] = i;
    }

    //create hashmap of parcel to leaf node value
    unordered_map<int, int> parcelToLeaf;

    // to store leaf node values in order of appearance
    vector<int> leaf_list; 

    TreeNode* root = buildTreeHelper(preorder, 0, preorder.size() - 1, inorder, 0, inorder.size() - 1, inMap, leaf_list);
    
    // free memory of inMap as it is no longer needed
    unordered_map<int, int>().swap(inMap);

    // sort leaflist to map parcels to leaf nodes in order of appearance
    sort(leaf_list.begin(), leaf_list.end());
    int numLeaves = leaf_list.size();

    //Assign parcels to leaf nodes in order of appearance
    for (int i = 0; i < numLeaves; ++i) {
        int leafNodeVal = leaf_list[i];
        for (int parcel : leafParcels[i]) {
            parcelToLeaf[parcel] = leafNodeVal;
        }
    }

    leaf_list.clear();           // removes all elements, but capacity is still reserved
    leaf_list.shrink_to_fit();   // requests STL to release unused capacity


    // Store path and path lenght from root to each leaf node
    unordered_map<int, vector<int>> leafPaths;
    vector<int> currentPath;

    storePaths(root, leafPaths, currentPath);
    
    // create results vector
    vector<int> results;

    // For each query, find the highest junction node
    for (const auto& q : query) {
        int junction = findHighestJunction(root, parcelToLeaf, leafPaths, q);
        results.push_back(junction);
    }

    return results;
}


/* 
Complete this function
*/

// Disjoint Set Union (DSU) for connectivity check
class DSU {
private:
    vector<int> parent, rank;
    
public:
    DSU(int n) {
        parent.resize(n + 1);
        rank.resize(n + 1, 0);
        for (int i = 1; i <= n; i++) {
            parent[i] = i;
        }
    }
    
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]); // Path compression
        }
        return parent[x];
    }
    
    void unite(int x, int y) {
        int px = find(x), py = find(y);
        if (px == py) return;
        
        // Union by rank
        if (rank[px] < rank[py]) {
            parent[px] = py;
        } else if (rank[px] > rank[py]) {
            parent[py] = px;
        } else {
            parent[py] = px;
            rank[px]++;
        }
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

struct State {
    int node;
    int time;
    bool refueled;
    
    State(int n, int t, bool r) : node(n), time(t), refueled(r) {}
};

struct Compare {
    bool operator()(const State& a, const State& b) {
        if (a.time != b.time) {
            return a.time > b.time; // Min-heap based on time (primary)
        }
        if (a.refueled != b.refueled) {
            return a.refueled < b.refueled; // Prioritize refueled state (secondary)
        }
        return a.node > b.node; // Smaller node ID first (tertiary)
    }
};

// Function to run Dijkstra from a starting node
vector<vector<int>> dijkstra(int start, int n, const unordered_map<int, vector<pair<int, int>>>& graph, const unordered_set<int>& metroSet) {
    
    // dist[node][refueled] = minimum time to reach node with/without refueling
    vector<vector<int>> dist(n + 1, vector<int>(2, INT_MAX));
    priority_queue<State, vector<State>, Compare> pq;
    
    // Initialize starting state - always can start without refueling
    pq.push(State(start, 0, false));
    dist[start][0] = 0;
    
    // If starting node is a metro city, can also start with refueling
    if (metroSet.count(start)) {
        pq.push(State(start, 0, true));
        dist[start][1] = 0;
    }
    
    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();
        
        int node = current.node;
        int time = current.time;
        bool refueled = current.refueled;
        
        // Skip if we've found a better path to this state
        if (time > dist[node][refueled]) continue;
        
        // Try refueling at current node if it's a metro city and not already refueled
        if (!refueled && metroSet.count(node)) {
            if (time < dist[node][1]) {
                dist[node][1] = time;
                pq.push(State(node, time, true));
            }
        }
        
        // Explore neighboring nodes
        auto it = graph.find(node);
        if (it != graph.end()) {
            for (auto& edge : it->second) {
                int neighbor = edge.first;
                int roadTime = edge.second;
                
                // If refueled, travel time is halved
                int travelTime = refueled ? roadTime / 2 : roadTime;
                int newTime = time + travelTime;
                
                if (newTime < dist[neighbor][refueled]) {
                    dist[neighbor][refueled] = newTime;
                    pq.push(State(neighbor, newTime, refueled));
                }
            }
        }
    }
    
    return dist;
}


long long question_three(const vector<vector<int>>& edges,const vector<int>& metro_cities) 
{
    // TODO: Implement function

    // Determine number of nodes from edges and metro_cities
    int n = 0;

    // Find maximum node number from edges
    for (const auto& edge : edges) {
        n = max(n, max(edge[0], edge[1]));
    }

    // Also check metro_cities in case some metro cities are not in edges
    for (int metro : metro_cities) {
        n = max(n, metro);
    }

    // Build adjacency list
    unordered_map<int, vector<pair<int, int>>> graph;
    // graph[node] = vector of (neighbor, travel_time)
    
    // Scope for DSU - will be destroyed after connectivity check
    {
        // Early fail case: Check if nodes 1 and n are connected using DSU
        DSU dsu(n);
        
        // Populate DSU while building graph
        for (auto& edge : edges) {
            int u = edge[0], v = edge[1], w = edge[2];
            graph[u].push_back({v, w});
            graph[v].push_back({u, w});
            dsu.unite(u, v); // Connect components
        }
        
        // Early return if nodes 1 and n are not connected
        if (!dsu.connected(1, n)) {
            return -1;
        }
        // DSU goes out of scope here and memory is freed
    }
    
    // Convert metro cities to set for O(1) lookup
    unordered_set<int> metroSet(metro_cities.begin(), metro_cities.end());
    
    // Run Dijkstra from both starting points
    vector<vector<int>> distFrom1 = dijkstra(1, n, graph, metroSet);
    vector<vector<int>> distFromN = dijkstra(n, n, graph, metroSet);
    
    int minMeetTime = INT_MAX;
    // Check all possible meeting points
    for (int meetNode = 1; meetNode <= n; meetNode++) {
        
        // Calculate the best time possible for both to reach meetNode by considering both refueled and non-refueled states
        int timeFrom1 = min(distFrom1[meetNode][0], distFrom1[meetNode][1]);
        int timeFromN = min(distFromN[meetNode][0], distFromN[meetNode][1]);
        
        // The meeting time is the max of both times since one can wait for the other arrival
        int meetTime = max(timeFrom1, timeFromN);

        // Update global minimum meeting time
        minMeetTime = min(minMeetTime, meetTime);
    }

    // If no valid meeting time was found, return -1
    return (minMeetTime == INT_MAX) ? -1 : minMeetTime;
}

#endif // USER_CODE_H
