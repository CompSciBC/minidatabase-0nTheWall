#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        //TODO
        heap.push_back(recIn);
        int rId = heap.size() - 1;
        idIndex.insert(recIn.id, rId);
        vector<int> lId;
        lId.push_back(rId);
        lastIndex.insert(toLower(recIn.last), lId);

        return heap[rId].id;//or is it just rId?

    }
/*
delete from idIndex
delete from lastIndex
delete from heap (mark deleted)like change deleted to true
*/
    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        //TODO

        for(int i = 0; i < heap.size(); i++){
            if(heap[i].id == id){
                heap[i].deleted = true;
                idIndex.erase(id);
                lastIndex.erase(toLower(heap[i].last));//erases entire last name entry but does not update
                for(int j = 0; j < heap.size(); j++){
                    if(toLower(heap[j].last) == toLower(heap[i].last) && heap[j].deleted == false){
                        vector<int> lId;
                        lId.push_back(j);
                        lastIndex.insert(toLower(heap[j].last), lId);
                        idIndex.insert(heap[j].id, j);
                    }
                }
                return true;
            }
        }
        return false;

    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        //TODO    
        cmpOut = 0;
		idIndex.resetMetrics();
		if(idIndex.find(id) != nullptr){
			cmpOut = idIndex.comparisons;
            std::cout<<"findById 1 number of comparisons: "<<cmpOut<<std::endl;
			idIndex.resetMetrics();
			//return idIndex.find(id);
            int rId = *(idIndex.find(id));
            return &heap[rId];
		}else{
			cmpOut = idIndex.comparisons;
            std::cout<<"findById 2 number of comparisons: "<<cmpOut<<std::endl;
			idIndex.resetMetrics();
            return nullptr;
        }
    }
    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        //TODO
        cmpOut = 0;
		idIndex.resetMetrics();
		vector<const Record*> out;
		idIndex.rangeApply(lo,hi, [&](const int &k, int &rId){
			if(rId >= 0 && rId < (int)heap.size() && !heap[rId].deleted)
				out.push_back(&heap[rId]);
		});

        cmpOut = idIndex.comparisons;
        std::cout<<"rangeById number of comparisons: "<<cmpOut<<std::endl;
        idIndex.resetMetrics();
		
        return out;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        //TODO
        cmpOut = 0;
		lastIndex.resetMetrics();
        vector<const Record*> out;
        lastIndex.rangeApply(toLower(prefix), toLower(prefix) + char(127), [&](const string &k, vector<int> &rIds){
            for(int rId : rIds){
                if(rId >= 0 && rId < (int)heap.size() && !heap[rId].deleted)
                    out.push_back(&heap[rId]);
                    std::cout<<"Out vector thing test: "<<out[0]<<" rId = "<<rId<<std::endl;

            }
        });

        cmpOut = lastIndex.comparisons;
        cout<<"prefixByLast number of comparisons: "<<cmpOut<<endl;
        lastIndex.resetMetrics();

        return out;

    }
};

#endif
