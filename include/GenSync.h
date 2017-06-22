/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// GenSync.h -- The general synchronization framework
#ifndef GEN_SYNC_H
#define GEN_SYNC_H

// standard libraries
#include <list>
#include <vector>
#include <string>

#include "Communicant.h"
#include "DataObject.h"
#include "SyncMethod.h"

// namespace info
using std::string;
using std::cout;
using std::clog;
using std::endl;
using std::list;
using std::vector;
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::ios;
using std::invalid_argument;
using std::runtime_error;

/**
 * Implements a data structure for storing sets of data
 * in a manner that is designed for efficient synchronization.
 * This is a generic container that is subclassed by different types of
 * synchronization options.
 * 
 * A Generic Synchronization object contains:
 * 0.  One or more communicants, which govern(s) the targets of data synchronization with this object (and includes a means of communicating with them).
 * 1.  One or more synchronization methods, which govern(s) the protocol through which synchronization is carried out.
 * 2.  Data that is associated with the local object.
 */
class GenSync {
public:
    /**
     * Specific GenSync constructor
     * @param cVec      The vector of other GenSync's with whom this data structure might
     *                      be synchronized.
     * @param mVec      The vector of synchronization methods that this GenSync
     *                      should be prepared to use.  The order of these methods
     *                      is significant.
     * @param data       The initial data with which to populate the data structure.  The data is added element by element
     *                      so that synchronization method metadata can be properly maintained.
     * 
     */
    GenSync(const vector<Communicant*> cVec, const vector<SyncMethod*> mVec, const list<DataObject*> data);

    /**
     * Specific GenSync constructor
     * @param cVec      The vector of other GenSync's with whom this data structure might
     *                      be synchronized.
     * @param mVec      The vector of synchronization methods that this GenSync
     *                      should be prepared to use.  The order of these methods
     *                      is significant.
     * @param fileName   The name of a file from which to read (line by line) initial elements of
     *                   this data structure.  As elements are added to this data structure, they
     *                   are also stored in the file.
     */
    GenSync(const vector<Communicant*> cVec, const vector<SyncMethod*> mVec, string fileName = "");


    // DATA MANIPULATION
    /**
     * Adds a new datum into the existing GenSync data structure
     * @param newDatum   The datum to be added
     * %R:  newDatum cannot have size larger than a long
     * %M:  If a file is associated with this object, then updates are stored in that file.
     */
    void addElem(DataObject* newDatum);
    
    /**
     * Adds a new datum into the existing GenSync data structure
     * @param newDatum The datum to be added ... must be of a type compatible with
     * the global toStr templated function.
     * %R:  newDatum cannot have size larger than a long
     * %M:  If a file is associated with this object, then updates are stored in that file.
     *      *    */
    template <typename T>
    void addElem(T* newDatum);

    /**
     * Deletes a given element from the GenSync data structure
     * Not currently implemented.
     * @unimplemented
     */
    void delElem(DataObject* newDatum);

    /**
     * @return a list of pointers to the elements stored in the data structure
     */// get a data object element from data list
    const list<DataObject *> dumpElements();


    // COMMUNICANT MANIPULATION
    /* Communicants are entities that can communicate [and thus sync] with this GenSync object.
     *  They must each be registered before synchronization.
     *  The first communicant gets index 0.
     */

    /**
     * Registers another communicant for possible synchronization.
     * @param newComm   The communicant that could synchronize its data with this object.
     * @param index     The index in the communicant vector at which to insert this communicant.
     *                  The order of communicants dictates the order in which they are
     *                  synchronized upon a synchronization call.
     *                  By default, new communicants are added to the back of the vector
     */
    void addComm(Communicant* newComm, int index = 0);

    /**
     * Delete all communicants oldComm (i.e. stored at the same memory address) from the communicant vector.
     * @param oldComm  A pointer to the desired communicant.
     */
    void delComm(Communicant* oldComm);

    /**
     * Delete the communicant at the given index in the communicant vector.
     * @param index  Index of the communicant to delete.
     */
    void delComm(int index);

    /**
     * @return The number of communicants currently registered. 
     */
    int numComm();


    // SYNCHRONIZATION AGENT MANIPULATION
    /* Synchronization agents represent the type of synchronization that could be
     * used to synchronize the data on communicants.  For example, the CPISync
     * synchronization agent uses the CPISync algorithm to synchronize communicant data.
     *  The first agent gets index 0.
     */

    /**
     * Registers another synchronization agent.
     * @param newAgent  The synchronization agent to add
     * @param index     The index in the agent vector at which to insert this communicant.
     *                  The order of agents is not significant.
     *                  By default, new agents are added to the back of the sync vector
     */
    void addSyncAgt(SyncMethod* newAgt, int index = 0);
    
    /**
     * Delete the agent at the given index in the agent vector.
     * @param index  Index of the agent to delete.
     */
    void delSyncAgt(int index);
    
    /**
     * 
     * @param index The index of the agent to return
     * @return The ii-th Sync Agent attached to this object
     */
    vector<SyncMethod*>::iterator getSyncAgt(int index);



    // SYNCHRONIZATION METHODS
    /**
     * Sequentially listens for a synchronization request from each communicant.
     * In other words, listens and waits for a synchronization request from the first
     * communicant.  When that request comes through and synchronization is complete,
     * listens for a request from the second communicant ...
     * @param sync_num  The method of synchronization request to listen for.  This is
     *          an index into the vector of synchronization methods supplied upon construction.
     *          Thus, if the first synchronization method supplied in the Constructor is
     *          a CPISync method, then sync_num=0 (the default value) will listen for a CPISync sync request.
     * @return true iff all synchronizations were completed successfully
     */
    bool listenSync(int sync_num = 0);

    /**
     * Sequentially sends a specific synchronization request to each communicant.
     * @param sync_num  This is an index into the vector of synchronization methods supplied upon construction.
     *          Thus, if the first synchronization method supplied in the constructor is
     *          a CPISync method, then sync_num=0 (the default value) will listen for a CPISync sync request.
     * @return           true iff all synchronizations were completed successfully
     */
    bool startSync(int sync_num);



    // INFORMATIONAL
    
    /**
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @return The number of bytes transmitted by the Communicant and index #commIndex.
     */
    const long getXmitBytes(int commIndex) const;
    
    /**
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @return The number of bytes received by the Communicant and index #commIndex.
     */
    const long getRecvBytes(int commIndex) const;
 
   /**
     * @param commIndex The index of the Communicant to query (in the order that they were added)
     * @return The amount of CPU time (in seconds) since the last sync request by the Communicant and index #commIndex.
     *          if there was no sync request, the creation time is used.
     */
    const double getSyncTime(int commIndex) const;
    
    /**
     * @return the port on which the server is listening for communicant comm_index.
     * If no server is listening for this communicant, the port returned is -1
     * @param comm_index       The index of the communicant that interests us.
     * */
    int getPort(int comm_index);
    
    /**
     * Displays some internal information about this method
     */
    string info() {
        return "I am a GenSync object";
    }
    
    /**
     * Destructor
     */
    ~GenSync();
    
private:
    // METHODS
    /**
     * No argument constructor ... should not be used
     */
    GenSync();
    
    
    // FIELDS
    /** A container for the data stored by this GenSync object. */
    list<DataObject*> myData;
    
    /** A vector of communicants registered to be able to sync with this GenSync object. */
    vector<Communicant*> myCommVec;
    
    /** A vector of synchronization methods that can be used to sync with this GenSync object. */
    vector<SyncMethod*> mySyncVec;
    
    /** The file to which to output any additions to the data structure. */
    ofstream *outFile;
};

#endif
