#include "XdmfArray.hpp"

int main(int argc, char* argv[])
{

	/**
	 * Test simple copy from array
	 */
	double values[] = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9};
	boost::shared_ptr<XdmfArray> array = XdmfArray::New();
	array->insertValues(0, &values[0], 9);
	assert(array->getSize() == 9);
	assert(array->getType().compare("Float") == 0);
	assert(array->getPrecision() == 8);
	assert(array->getValues().compare("1.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 ") == 0);
	// Now modify original array
	values[0] = 0;
	assert(array->getValues().compare("1.1 2.2 3.3 4.4 5.5 6.6 7.7 8.8 9.9 ") == 0);

	/**
	 * Test simple copy from vector
	 */
	std::vector<int> values2;
	values2.push_back(100);
	values2.push_back(200);
	boost::shared_ptr<XdmfArray> array2 = XdmfArray::New();
	array2->copyValues(values2);
	assert(array2->getSize() == 2);
	assert(array2->getType().compare("Int") == 0);
	assert(array2->getPrecision() == 4);
	assert(array2->getValues().compare("100 200 ") == 0);
	// Now modify original array
	values2.push_back(300);
	assert(array2->getSize() == 2);
	assert(array2->getType().compare("Int") == 0);
	assert(array2->getPrecision() == 4);
	assert(array2->getValues().compare("100 200 ") == 0);

	/**
	 * Test shared vector assignment
	 */
	boost::shared_ptr<std::vector<char> > values3(new std::vector<char>());
	values3->push_back(-2);
	values3->push_back(-1);
	values3->push_back(0);
	values3->push_back(1);
	values3->push_back(2);
	boost::shared_ptr<XdmfArray> array3 = XdmfArray::New();
	array3->setValues(values3);
	assert(array3->getSize() == 5);
	assert(array3->getType().compare("Char") == 0);
	assert(array3->getPrecision() == 1);
	assert(array3->getValues().compare("-2 -1 0 1 2 ") == 0);
	// Now modify original array
	values3->push_back(8);
	assert(array3->getSize() == 6);
	std::cout << array3->getValues() << std::endl;
	assert(array3->getValues().compare("-2 -1 0 1 2 8 ") == 0);

	return 0;
}
