#ifndef XDMFDATAITEM_HPP_
#define XDMFDATAITEM_HPP_

// Forward Declarations
class XdmfArray;

// Includes
#include "XdmfItem.hpp"

/**
 * @brief A type of XdmfItem that holds data values in an XdmfArray.
 *
 * XdmfDataItem is an abstract base class.  Any part of the Xdmf graph structure that holds values
 * in an XdmfArray inherits from this.  XdmfDataItem imbues the ability to store and retrieve XdmfArrays.
 */
class XdmfDataItem : public XdmfItem {

public:

	virtual ~XdmfDataItem();

	LOKI_DEFINE_VISITABLE(XdmfDataItem, XdmfItem)

	/**
	 * Get the XdmfArray attached to this XdmfDataItem.
	 *
	 * @return a smart pointer to the XdmfArray.
	 */
	boost::shared_ptr<XdmfArray> getArray();

	/**
	 * Get the XdmfArray attached to this XdmfDataItem (const version).
	 *
	 * @return a smart pointer to the XdmfArray.
	 */
	boost::shared_ptr<const XdmfArray> getArray() const;

	/**
	 * Attach an XdmfArray to this XdmfDataItem.
	 *
	 * @param array a smart pointer to the XdmfArray.
	 */
	void setArray(const boost::shared_ptr<XdmfArray> array);

	virtual void traverse(const boost::shared_ptr<XdmfBaseVisitor> visitor) const;

protected:

	XdmfDataItem();
	virtual void populateItem(const std::map<std::string, std::string> & itemProperties, std::vector<boost::shared_ptr<XdmfItem > > & childItems);

private:

	XdmfDataItem(const XdmfDataItem & dataItem);  // Not implemented.
	void operator=(const XdmfDataItem & dataItem);  // Not implemented.

	boost::shared_ptr<XdmfArray> mArray;
};

#endif /* XDMFDATAITEM_HPP_ */