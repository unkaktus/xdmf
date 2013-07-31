#include "XdmfReader.hpp"
#include "XdmfArray.hpp"
#include "XdmfDomain.hpp"
#include "XdmfItem.hpp"


int main(int, char **)
{
	//#initialization begin

	//using XdmfReader since XdmfCoreReader is abstract
	shared_ptr<XdmfReader> exampleReader = XdmfReader::New();

	//#initialization end

	//#parse begin

	std::string readLight = "your light data here";
	//Assuming that an XdmfArray is the root item generated by the light data provided
	shared_ptr<XdmfArray> exampleArray = shared_dynamic_cast<XdmfArray>(reader->parse(readLight));

	//#parse end

	//#readpath begin

	std::string readPath = "your file path here";

	//#readpath end

	//#readroot begin

	//Assuming that an XdmfDomain is the root item at the file path provided
	shared_ptr<XdmfDomain> exampleDomain = shared_dynamic_cast<XdmfDomain>(reader->read(readPath));

	//#readroot end

	//#readItems begin

	std::vector<shared_ptr<XdmfItem> > exampleCollection = reader->readItems(readPath);
	//Used in a similar manner as read, but in this case the read file has multiple items at the returned level

	//#readItems end

	//#readXPath begin

	std::string readXPath = "your X path here";

	std::vector<shared_ptr<XdmfItem> > exampleItems = reader->read(readPath, readXPath);

	//#readXPath end

	return 0;
}
