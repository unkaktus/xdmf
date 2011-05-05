#include "XdmfDomain.hpp"
#include "XdmfGeometry.hpp"
#include "XdmfGeometryType.hpp"
#include "XdmfReader.hpp"
#include "XdmfTopology.hpp"
#include "XdmfTopologyType.hpp"
#include "XdmfUnstructuredGrid.hpp"
#include "XdmfWriter.hpp"

#include "XdmfTestCompareFiles.hpp"

int main(int, char *)
{

  //
  // Create a unstructured grid consisting of a quadrilateral element
  // and a polyline element connected to each other by a single node at 
  // the corner of each.
  // 
  boost::shared_ptr<XdmfUnstructuredGrid> grid = XdmfUnstructuredGrid::New();
  grid->setName("Mixed");

  // Set Geometry
  double points[] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 2.0, 2.0, 3.0, 
                     1.0, 4.0, 2.0, 5.0, 1.0, 6.0, 2.0};
  grid->getGeometry()->setType(XdmfGeometryType::XY());
  grid->getGeometry()->insert(0, &points[0], 18);
  
  // Set Topology
  boost::shared_ptr<XdmfTopology> topology = grid->getTopology();
  topology->setType(XdmfTopologyType::Mixed());
  topology->pushBack(XdmfTopologyType::Quadrilateral()->getID());
  topology->pushBack(0);
  topology->pushBack(1);
  topology->pushBack(2);
  topology->pushBack(3);
  topology->pushBack(XdmfTopologyType::Polyline()->getID());
  topology->pushBack(6);
  topology->pushBack(2);
  topology->pushBack(4);
  topology->pushBack(5);
  topology->pushBack(6);
  topology->pushBack(7);
  topology->pushBack(8);

  assert(topology->getNumberElements() == 2);

  boost::shared_ptr<XdmfDomain> domain = XdmfDomain::New();
  domain->insert(grid);

  boost::shared_ptr<XdmfWriter> writer =
    XdmfWriter::New("TestXdmfTopologyMixed1.xmf");
  domain->accept(writer);

  boost::shared_ptr<XdmfReader> reader = XdmfReader::New();
  boost::shared_ptr<XdmfDomain> readDomain =
    boost::shared_dynamic_cast<XdmfDomain>
    (reader->read("TestXdmfTopologyMixed1.xmf"));

  boost::shared_ptr<XdmfWriter> writer2 =
    XdmfWriter::New("TestXdmfTopologyMixed2.xmf");
  readDomain->accept(writer2);

  assert(XdmfTestCompareFiles::compareFiles("TestXdmfTopologyMixed1.xmf",
                                            "TestXdmfTopologyMixed2.xmf"));

  return 0;
}