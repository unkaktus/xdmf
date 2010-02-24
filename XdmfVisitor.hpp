// Kenneth Leiter
// Xdmf Smart Pointer Test

#ifndef XDMFVISITOR_HPP_
#define XDMFVISITOR_HPP_

// Forward Declarations
class XdmfAttribute;
class XdmfDataItem;
class XdmfDomain;
class XdmfGeometry;
class XdmfGrid;
class XdmfTopology;

// Includes
#include <hdf5.h>
#include <sstream>
#include <vector>
#include "XdmfObject.hpp"

class XdmfVisitor : public XdmfObject {

public:

	XdmfNewMacro(XdmfVisitor);

	virtual void visit(const XdmfAttribute * const attribute);

	virtual void visit(const XdmfDataItem * const dataItem);

	virtual void visit(const XdmfDomain * const domain);

	virtual void visit(const XdmfGeometry * const geometry);

	virtual void visit(const XdmfGrid * const grid);

	virtual void visit(const XdmfTopology * const topology);

	virtual std::string printSelf() const;

protected:

	XdmfVisitor();
	virtual ~XdmfVisitor();

private:

	XdmfVisitor(const XdmfVisitor&);  // Not implemented.
	void operator=(const XdmfVisitor&);  // Not implemented.

	std::string getHDF5GroupName();

	int mTabIndex;
	std::stringstream xmlData;
	hid_t hdf5Handle;
	std::vector<std::string> dataHierarchy;
	int mLightDataLimit;
	std::string mHeavyFileName;

};

#endif /* XDMFVISITOR_HPP_ */
