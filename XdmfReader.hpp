#ifndef XDMFREADER_HPP_
#define XDMFREADER_HPP_

// Includes
#include "XdmfCoreReader.hpp"

/**
 * @brief Reads an Xdmf file stored on disk into memory.
 *
 * Reads an Xdmf file stored on disk into an Xdmf structure in memory.  All light data is parsed in order to create appropriate
 * Xdmf objects.  Heavy data controllers are created and attached to XdmfArrays but no heavy data is read into memory.
 */
class XdmfReader : public XdmfCoreReader {

public:

	XdmfNewMacro(XdmfReader);
	virtual ~XdmfReader();

	boost::shared_ptr<XdmfItem> read(const std::string & fileName) const;

protected:

	XdmfReader();

private:

	XdmfReader(const XdmfReader & reader);  // Not implemented.
	void operator=(const XdmfReader & reader);  // Not implemented.
};

#endif /* XDMFREADER_HPP_ */