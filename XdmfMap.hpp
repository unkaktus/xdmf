/*****************************************************************************/
/*                                    XDMF                                   */
/*                       eXtensible Data Model and Format                    */
/*                                                                           */
/*  Id : XdmfMap.hpp                                                         */
/*                                                                           */
/*  Author:                                                                  */
/*     Kenneth Leiter                                                        */
/*     kenneth.leiter@arl.army.mil                                           */
/*     US Army Research Laboratory                                           */
/*     Aberdeen Proving Ground, MD                                           */
/*                                                                           */
/*     Copyright @ 2011 US Army Research Laboratory                          */
/*     All Rights Reserved                                                   */
/*     See Copyright.txt for details                                         */
/*                                                                           */
/*     This software is distributed WITHOUT ANY WARRANTY; without            */
/*     even the implied warranty of MERCHANTABILITY or FITNESS               */
/*     FOR A PARTICULAR PURPOSE.  See the above copyright notice             */
/*     for more information.                                                 */
/*                                                                           */
/*****************************************************************************/

#ifndef XDMFMAP_HPP_
#define XDMFMAP_HPP_

// Forward Declarations
class XdmfArray;
class XdmfAttribute;
class XdmfHeavyDataController;

// Includes
#include "Xdmf.hpp"
#include "XdmfItem.hpp"

#include <set>

/**
 * @brief Boundary communicator map for partitioned spatial
 * collections.
 *
 * Provides mechanism for mapping nodes across partition
 * boundaries. Each partitioned grid contains its own map, mapping its
 * own nodes to all other nodes in the global system.
 *
 * There are two methods for constructing XdmfMaps:
 *
 * Calling New() with no parameters will construct an empty map. The
 * map can be filled manually with subsequent insert commands.
 *
 * Calling New(const std::vector<shared_ptr<XdmfAttribute> > &
 * globalNodeIds) will construct XdmfMaps for each grid in an entire
 * global system. Each entry in the vector contains the globalNodeIds
 * for that partition. The constructor accepts global node ids for
 * each partition to construct the proper XdmfMaps.
 */
class XDMF_EXPORT XdmfMap : public XdmfItem {

public:

  typedef int node_id;
  typedef int task_id;
  typedef std::map<node_id, std::set<node_id> > node_id_map;

  /**
   * Create a new XdmfMap.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline New
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline New
   *
   * @return constructed XdmfMap.
   */
  static shared_ptr<XdmfMap> New();

  /**
   * Create XdmfMaps for each grid in a domain decomposed mesh. Each
   * entry in the globalNodeIds vector contains the global node ids
   * for that partition.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline holdGlobalNodes
   * @until localNodeID)
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline #create
   * @until push_back(map2Attribute)
   * @skipline New
   * @until localNodeID)
   *
   * @param globalNodeIds a vector of attributes containing globalNodeId
   * values for each partition to be mapped.
   *
   * @return constructed XdmfMaps for each partition. The size of the vector
   * will be the same as the globalNodeIds vector.
   */
  static std::vector<shared_ptr<XdmfMap> >
  New(const std::vector<shared_ptr<XdmfAttribute> > & globalNodeIds);

  virtual ~XdmfMap();

  LOKI_DEFINE_VISITABLE(XdmfMap, XdmfItem);
  static const std::string ItemTag;

  std::map<std::string, std::string> getItemProperties() const;

  /**
   * Get stored boundary communicator map.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skip getMap
   * @skipline Assuming
   * @until //remoteNodeValue
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skip getMap
   * @skipline Assuming
   * @until #prints
   *
   * @return stored boundary communicator map.
   */
  std::map<task_id, node_id_map> getMap() const;

  /**
   * Get name of boundary communicator map.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline Assumming
   * @skipline getName
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline Assumming
   * @skipline getName
   *
   * @return name of boundary communicator map.
   */
  std::string getName() const;

  /**
   * Given a remote task id return a map of local node ids to remote
   * node ids
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skip getMap
   * @skipline Assuming
   * @until 4)
   * @skipline getRemoteNodeIds
   * @until //remoteNodeValue
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skip getMap
   * @skipline Assuming
   * @until 4)
   * @skipline getRemoteNodeIds
   * @until #prints
   *
   * @param remoteTaskId a task id to retrieve mapping for.
   *
   * @return a map of local node ids to a vector of remote node ids on
   * remoteTaskId.
   */
  node_id_map getRemoteNodeIds(const task_id remoteTaskId);

  std::string getItemTag() const;

  using XdmfItem::insert;

  /**
   * Insert a new entry in map.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline New
   * @skipline newRemoteTaskID
   * @until //This
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline New
   * @skipline newRemoteTaskID
   * @until #This
   *
   * @param remoteTaskId task id where the remoteLoalNodeId is located.
   * @param localNodeId the node id of the node being mapped.
   * @param remoteLocalNodeId a node id on the remoteTaskId that the
   * localNodeId is mapped to.
   */
  void insert(const task_id  remoteTaskId,
              const node_id  localNodeId,
              const node_id  remoteLocalNodeId);

  /**
   * Returns whether the map is initialized (contains values in
   * memory).
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline Assumming
   * @skipline isInitialized
   * @until }
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline Assumming
   * @skipline isInitialized
   * @until read
   *
   * @return bool true if map contains values in memory.
   */
  bool isInitialized() const;

  /**
   * Read data from disk into memory.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline Assumming
   * @skipline isInitialized
   * @until }
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline Assumming
   * @skipline isInitialized
   * @until read
   */
  void read();

  /**
   * Release all data held in memory. The heavy data remain attached.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline Assumming
   * @skipline release
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline Assumming
   * @skipline release
   */
  void release();

  /**
   * Set the heavy data controllers for this map.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline hdf5FilePath
   * @until setHeavyDataController
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline hdf5FilePath
   * @until setHeavyDataController
   *
   * @param remoteTaskIdsControllers a vector of XdmfHeavyDataControllers to the remote
   * task ids dataset.
   * @param localNodeIdsControllers a vector of XdmfHeavyDataControllers to the local
   * node ids dataset.
   * @param remoteLocalNodeIdsControllers a vector of XdmfHeavyDataControllers to the
   * remote local node ids dataset.
   */
  void
  setHeavyDataControllers(std::vector<shared_ptr<XdmfHeavyDataController> > remoteTaskIdsControllers,
                          std::vector<shared_ptr<XdmfHeavyDataController> > localNodeIdsControllers,
                          std::vector<shared_ptr<XdmfHeavyDataController> > remoteLocalNodeIdsControllers);

  /**
   * Set the boundary communicator map.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline New
   * First create a std::map<int, std::set<int> >
   * @skipline taskMap
   * @until //Are
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline newTaskMap
   * @until #Is
   *
   * @param map the boundary communicator map to store.
   */
  void setMap(std::map<task_id, node_id_map> map);

  /**
   * Set the name of the boundary communicator map.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfMap.cpp
   * @skipline Assumming
   * @skipline newName
   * @until setName
   *
   * Python
   *
   * @dontinclude XdmfExampleMap.py
   * @skipline Assumming
   * @skipline newName
   * @until setName
   *
   * @param name the name of the boundary communicator map to set.
   */
  void setName(const std::string & name);

  void traverse(const shared_ptr<XdmfBaseVisitor> visitor);

protected:

  XdmfMap();

  virtual void
  populateItem(const std::map<std::string, std::string> & itemProperties,
               const std::vector<shared_ptr<XdmfItem> > & childItems,
               const XdmfCoreReader * const reader);

private:

  XdmfMap(const XdmfMap & map);  // Not implemented.
  void operator=(const XdmfMap & map);  // Not implemented.

  std::vector<shared_ptr<XdmfHeavyDataController> > mLocalNodeIdsControllers;
  // remoteTaskId | localNodeId | remoteLocalNodeId
  std::map<task_id, node_id_map > mMap;
  std::string mName;
  std::vector<shared_ptr<XdmfHeavyDataController> > mRemoteLocalNodeIdsControllers;
  std::vector<shared_ptr<XdmfHeavyDataController> > mRemoteTaskIdsControllers;

};

#endif /* XDMFMAP_HPP_ */
