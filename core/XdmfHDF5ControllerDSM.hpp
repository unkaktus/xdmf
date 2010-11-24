#ifndef XDMFHDF5CONTROLLERDSM_HPP_
#define XDMFHDF5CONTROLLERDSM_HPP_

// Forward Declarations
class H5FDdsmBuffer;

// Includes
#include "XdmfHDF5Controller.hpp"

/**
 * @brief Couples an XdmfArray with HDF5 data stored in a DSM buffer.
 *
 * Serves as an interface between data stored in XdmfArrays and data stored in DSM buffers.  When an Xdmf file is read from or
 * written to a DSM buffer an XdmfHDF5ControllerDSM is attached to XdmfArrays.  This allows data to be released
 * from memory but still be accessible or have its location written to light data.
 */
class XdmfHDF5ControllerDSM : public XdmfHDF5Controller {

public:

	virtual ~XdmfHDF5ControllerDSM();

	/**
	 * Create a new controller for an hdf5 data set on disk.
	 */
	static boost::shared_ptr<XdmfHDF5ControllerDSM> New(const std::string & hdf5FilePath, const std::string & dataSetPath, const unsigned int size, const boost::shared_ptr<const XdmfArrayType> type, H5FDdsmBuffer * const dsmBuffer);

	std::string getName() const;

	void read(XdmfArray * const array);

protected:

	XdmfHDF5ControllerDSM(const std::string & hdf5FilePath, const std::string & dataSetPath, const unsigned int size, const boost::shared_ptr<const XdmfArrayType> type, H5FDdsmBuffer * const dsmBuffer);

private:

	XdmfHDF5ControllerDSM(const XdmfHDF5Controller & hdf5Controller);  // Not implemented.
	void operator=(const XdmfHDF5Controller & hdf5Controller);  // Not implemented.

	H5FDdsmBuffer * mDSMBuffer;

};

#endif /* XDMFHDF5CONTROLLER_HPP_ */