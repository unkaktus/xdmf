/*****************************************************************************/
/*                                    XDMF                                   */
/*                       eXtensible Data Model and Format                    */
/*                                                                           */
/*  Id : XdmfFunction.cpp                                                    */
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


#include "XdmfArray.hpp"
#include "XdmfArrayType.hpp"
#include "XdmfFunction.hpp"
#include <stack>
#include <math.h>
#include <boost/assign.hpp>
#include "XdmfError.hpp"

std::string XdmfFunction::mSupportedOperations = "|#()";
const std::string XdmfFunction::mValidVariableChars =
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_:.";
const std::string XdmfFunction::mValidDigitChars = "-1234567890.";
// List the priorities for the operations, based on the order of operations
// The index of the corresponding operation in validOperationChars
// is the same as the index of its priority in this array
std::map<char, int> XdmfFunction::mOperationPriority = boost::assign::map_list_of ('|', 2) ('#', 1) ('(', 0) (')', 0);
// The higher the value, the earlier the operation is
// evaluated in the order of operations
// With the exception of parenthesis which are evaluated
// as soon as the closing parenthesis is found

// Note, it doesn't handle overloaded functions well.
// Will generate errors unless overload methods are typecast.
std::map<std::string, shared_ptr<XdmfArray> (*)(std::vector<shared_ptr<XdmfArray> >)>
  XdmfFunction::arrayFunctions =
    boost::assign::map_list_of ("SUM", (shared_ptr<XdmfArray> (*)(std::vector<shared_ptr<XdmfArray> >))XdmfFunction::sum)
                               ("AVE", (shared_ptr<XdmfArray> (*)(std::vector<shared_ptr<XdmfArray> >))XdmfFunction::average);
std::map<char, shared_ptr<XdmfArray> (*)(shared_ptr<XdmfArray>, shared_ptr<XdmfArray>)>
  XdmfFunction::operations = boost::assign::map_list_of ('|', XdmfFunction::chunk) ('#', XdmfFunction::interlace);





shared_ptr<XdmfFunction>
XdmfFunction::New()
{
  shared_ptr<XdmfFunction> p(new XdmfFunction());
  return p;
}

XdmfFunction::XdmfFunction()
{
}

XdmfFunction::~XdmfFunction()
{
}

const std::string XdmfFunction::ItemTag = "Function";

int
XdmfFunction::addFunction(std::string name,
                       shared_ptr<XdmfArray>(*functionref)(std::vector<shared_ptr<XdmfArray> >))
{
  // Check to ensure that the name has valid characters
  for (unsigned int i = 0; i < name.size(); ++i) {
    // If the character is not found in the list of valid characters
    if (mValidVariableChars.find(name[i]) == std::string::npos) {
      // Then throw an error
      try {
        XdmfError::message(XdmfError::FATAL,
                           "Error: Function Name Contains Invalid Character(s)");
      }
      catch (XdmfError e) {
        throw e;
      }
    }
  }
  size_t origsize = arrayFunctions.size();
  arrayFunctions[name] = functionref;
  // If no new functions were added
  if (origsize == arrayFunctions.size()) {
    // Toss a warning, it's nice to let people know that they're doing this
    try {
      XdmfError::message(XdmfError::WARNING,
                         "Warning: Function Overwritten");
    }
    catch (XdmfError e) {
      throw e;
    }
  }
  return arrayFunctions.size();
}

shared_ptr<XdmfArray>
XdmfFunction::average(std::vector<shared_ptr<XdmfArray> > values)
{
        double total = sum(values)->getValue<double>(0);;
        int totalSize = 0;
        for (unsigned int i = 0; i < values.size(); ++i)
        {
                totalSize += values[i]->getSize();
        }
        shared_ptr<XdmfArray> returnArray = XdmfArray::New();
        returnArray->insert(0, total/totalSize);
        return returnArray;
}

shared_ptr<XdmfArray>
XdmfFunction::chunk(shared_ptr<XdmfArray> val1, shared_ptr<XdmfArray> val2)
{
  // Join chunk (add the new array to the end of the first one)
  // Joins into new array and returns it
  shared_ptr<XdmfArray> returnArray = XdmfArray::New();
    // Determining what type to class it as in order to not lose data, and to still have the smallest data type of the two
  shared_ptr<const XdmfArrayType> resultType = XdmfArrayType::comparePrecision(val1->getArrayType(), val2->getArrayType());

  if (resultType == XdmfArrayType::Int8()) {
    char sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Int16()) {
    short sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Int32()) {
    int sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Int64()) {
    long sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::UInt8()) {
    unsigned char sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::UInt16()) {
    unsigned short sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::UInt32()) {
    unsigned int sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Float32()) {
    float sampleValue = 0.0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Float64()) {
    double sampleValue = 0.0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::String()) {
    std::string sampleValue = "";
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else {
    // error type not supported
    XdmfError::message(XdmfError::FATAL, "Invalid type during Interlace");
  }
  returnArray->insert(0, val1, 0, val1->getSize(),  1, 1);
  returnArray->insert(val1->getSize(), val2, 0, val2->getSize(), 1, 1);
  return returnArray;
}

// This function and the functions it depends on are reimplemented
// in XdmfCore.i in order to properly interface with python.
// There are differences between the versions,
// but the overall algorithm remains mostly the same.
shared_ptr<XdmfArray>
XdmfFunction::evaluateExpression(std::string expression,
                                 std::map<std::string,
                                 shared_ptr<XdmfArray> > variables)
{
  std::stack<shared_ptr<XdmfArray> > valueStack;
  std::stack<char> operationStack;

  // String is parsed left to right
  // Elements of the same priority are evaluated right to left
  for (unsigned int i = 0; i < expression.size(); ++i) {
    // Found to be a digit
    if (mValidDigitChars.find(expression[i]) != std::string::npos) {
      // Progress until a non-digit is found
      int valueStart = i;
      if (i + 1 < expression.size()) {
        while (mValidDigitChars.find(expression[i+1]) != std::string::npos) {
          i++;
        }
      }
      // Push back to the value stack
      shared_ptr<XdmfArray> valueArray = XdmfArray::New();
      // Use this to convert to double
      valueArray->insert(0, atof(expression.substr(valueStart, i + 1 - valueStart).c_str()));
      valueStack.push(valueArray);
    }
    else if (mValidVariableChars.find(expression[i]) != std::string::npos) {
      // Found to be a variable
      int valueStart = i;
      // Progress until a nonvariable value is found
      if (i+1 < expression.size()){
        while (mValidVariableChars.find(expression[i+1]) != std::string::npos) {
          i++;
        }
      }
      // Convert to equivalent
      if (variables.find(expression.substr(valueStart, i + 1 - valueStart))
          == variables.end()) {
        if (arrayFunctions.find(expression.substr(valueStart, i + 1 - valueStart))
            == arrayFunctions.end()) {
          try {
            XdmfError::message(XdmfError::FATAL,
                               "Error: Invalid Variable in evaluateExpression "
                               + expression.substr(valueStart, i + 1 - valueStart));
          }
          catch (XdmfError e) {
            throw e;
          }
        }
        else {
          std::string currentFunction = expression.substr(valueStart, i + 1 - valueStart);
          // Check if next character is an open parenthesis
          if (i+1 >= expression.size()) {
            if (expression[i+1] != '(') {
              try {
                XdmfError::message(XdmfError::FATAL,
                                   "Error: No values supplied to function "
                                   + expression.substr(valueStart, i + 1 - valueStart));
              }
              catch (XdmfError e) {
                throw e;
              }
            }
          }
          // If it is grab the string between paranthesis

          if (i + 2 >= expression.size()) {
            XdmfError::message(XdmfError::FATAL,
                               "Error: Missing closing parethesis to function "
                               + expression.substr(valueStart, i + 1 - valueStart));
          }
          i = i + 2;
          valueStart = i;
          int numOpenParenthesis = 0;
          while ((expression[i] != ')' || numOpenParenthesis) && i < expression.size()) {
            if (expression[i] == '(') {
              numOpenParenthesis++;
            }
            else if (expression[i] == ')') {
              numOpenParenthesis--;
            }
            i++;
          }
          std::string functionParameters = expression.substr(valueStart, i - valueStart);
          std::vector<shared_ptr<XdmfArray> > parameterVector;
          // Split that string at commas
          size_t parameterSplit = 0;
          while (parameterSplit != std::string::npos) {
            parameterSplit = 0;
            parameterSplit = functionParameters.find_first_of(",", parameterSplit);
            // Feed the substrings to the parse function
            if (parameterSplit == std::string::npos) {
              parameterVector.push_back(evaluateExpression(functionParameters, variables));
            }
            else {
              parameterVector.push_back(evaluateExpression(functionParameters.substr(0, parameterSplit), variables));
              functionParameters = functionParameters.substr(parameterSplit+1);
            }
          }
          valueStack.push(evaluateFunction(parameterVector, currentFunction));
        }
      }
      else {
        // Push equivalent to value stack
        valueStack.push(variables.find(expression.substr(valueStart, i + 1 - valueStart))->second);
      }
    }
    else if (mSupportedOperations.find(expression[i]) != std::string::npos) {
      // Found to be an operation
      // Ppop operations off the stack until one of a lower or equal importance is found
      if (operationStack.size() > 0) {
        if (expression[i] == ')') {
          // To close a parenthesis pop off all operations until another parentheis is found
          while (operationStack.size() > 0 && operationStack.top() != '(') {
            // Must be at least two values for this loop to work properly
            if (valueStack.size() < 2) {
              try {
                XdmfError::message(XdmfError::FATAL,
                                   "Error: Not Enough Values in evaluateExpression");
              }
              catch (XdmfError e) {
                throw e;
              }
            }
            else {
              shared_ptr<XdmfArray> val2 = valueStack.top();
              valueStack.pop();
              shared_ptr<XdmfArray> val1 = valueStack.top();
              valueStack.pop();
              valueStack.push(evaluateOperation(val1, val2, operationStack.top()));
              operationStack.pop();
            }
          }
          operationStack.pop();
        }
        else if (expression[i] == '(') {
          // Just add it if it's a start parenthesis
          // Nothing happens here in that case
          // Addition happens after the if statement
        }
        else {
          int operationLocation = getOperationPriority(expression[i]);
          int topOperationLocation = getOperationPriority(operationStack.top());
          // See order of operations to determine importance
          while (operationStack.size() > 0 && operationLocation < topOperationLocation) {
            // Must be at least two values for this loop to work properly
            if (valueStack.size() < 2) {
              try {
                XdmfError::message(XdmfError::FATAL,
                                   "Error: Not Enough Values in evaluateExpression");
              }
              catch (XdmfError e) {
                throw e;
              }
            }
            else {
              shared_ptr<XdmfArray> val2 = valueStack.top();
              valueStack.pop();
              shared_ptr<XdmfArray> val1 = valueStack.top();
              valueStack.pop();
              valueStack.push(evaluateOperation(val1, val2, operationStack.top()));
              operationStack.pop();
              if (operationStack.size() == 0) {
                break;
              }
              topOperationLocation = getOperationPriority(operationStack.top());
            }
          }
        }
      }
      if (expression[i] != ')') {
        // Add the operation to the operation stack
        operationStack.push(expression[i]);
      }
    }
    // If not a value or operation the character is ignored
  }

  // Empty what's left in the stacks before finishing
  while (valueStack.size() > 1 && operationStack.size() > 0) {
    if (valueStack.size() < 2) {
      // Must be at least two values for this loop to work properly
      try {
        XdmfError::message(XdmfError::FATAL,
                           "Error: Not Enough Values in evaluateExpression");
      }
      catch (XdmfError e) {
        throw e;
      }
    }
    else {
      if(operationStack.top() == '(') {
        try {
          XdmfError::message(XdmfError::WARNING,
                             "Warning: Unpaired Parenthesis");
        }
        catch (XdmfError e) {
          throw e;
        }
      }
      else {
        shared_ptr<XdmfArray> val2 = valueStack.top();
        valueStack.pop();
        shared_ptr<XdmfArray> val1 = valueStack.top();
        valueStack.pop();
        if (operationStack.size() == 0) {
          try {
            XdmfError::message(XdmfError::FATAL,
                               "Error: Not Enough Operators in evaluateExpression");
          }
          catch (XdmfError e) {
            throw e;
          }
        }
        else {
          valueStack.push(evaluateOperation(val1, val2, operationStack.top()));
          operationStack.pop();
        }
      }
    }
  }

  // Throw error if there's extra operations
  if (operationStack.size() > 0) {
    try {
      XdmfError::message(XdmfError::WARNING,
                         "Warning: Left Over Operators in evaluateExpression");
    }
    catch (XdmfError e) {
      throw e;
    }
  }

  if (valueStack.size() > 1) {
    try {
      XdmfError::message(XdmfError::WARNING,
                         "Warning: Left Over Values in evaluateExpression");
    }
    catch (XdmfError e) {
      throw e;
    }
  }

  return valueStack.top();
}

shared_ptr<XdmfArray>
XdmfFunction::evaluateOperation(shared_ptr<XdmfArray> val1,
                             shared_ptr<XdmfArray> val2,
                             char operation)
{
  if (operations.find(operation) != operations.end()) {
    return (*(shared_ptr<XdmfArray>(*)(shared_ptr<XdmfArray>,
                                       shared_ptr<XdmfArray>))operations[operation])(val1,
                                                                                     val2);
  }
  else {
    return shared_ptr<XdmfArray>();
  }
}

int
XdmfFunction::addOperation(char newoperator,
                        shared_ptr<XdmfArray>(*operationref)(shared_ptr<XdmfArray>,
                                                             shared_ptr<XdmfArray>),
                        int priority)
{
  if (newoperator == '(' || newoperator == ')') {
    try {
      XdmfError::message(XdmfError::FATAL,
                         "Error: Parenthesis can not be redefined");
    }
    catch (XdmfError e) {
      throw e;
    }
  }
  if (mValidVariableChars.find(newoperator) != std::string::npos
      || mValidDigitChars.find(newoperator) != std::string::npos) {
    try {
      XdmfError::message(XdmfError::FATAL,
                         "Error: Operation Overlaps with Variables");
    }
    catch (XdmfError e) {
      throw e;
    }
  }
  // Give warning if the operation already exists
  size_t origsize = operations.size();
  // Place reference in the associated location
  operations[newoperator] = operationref;
  // It's nice to let people know they're doing this
  // So they don't get surprised about changes in behavior
  if (origsize == operations.size()) {
    try {
      XdmfError::message(XdmfError::WARNING,
                         "Warning: Operation Overwritten");
    }
    catch (XdmfError e) {
      throw e;
    }
    // Overwrite the existing info for that operation
    // Add the priority to the specified location in the priority array
    mOperationPriority[newoperator] = priority;
  }
  else {
    // Create new operation
    // Add operation to the supported character string
    mSupportedOperations.push_back(newoperator);
    mOperationPriority[newoperator] = priority;
  }
  return operations.size();
}

// This is how you use references to functions
shared_ptr<XdmfArray>
XdmfFunction::evaluateFunction(std::vector<shared_ptr<XdmfArray> > valueVector,
                            std::string functionName)
{
  if (arrayFunctions.find(functionName) != arrayFunctions.end()) {
    return (*(shared_ptr<XdmfArray>(*)(std::vector<shared_ptr<XdmfArray> >))arrayFunctions[functionName])(valueVector);
  }
  else {
    return shared_ptr<XdmfArray>();
  }
}

std::string
XdmfFunction::getItemTag() const
{
  return ItemTag;
}

std::map<std::string, std::string>
XdmfFunction::getItemProperties() const
{
  std::map<std::string, std::string> functionProperties;
  return functionProperties;
}

int
XdmfFunction::getOperationPriority(char operation)
{
  size_t operationLocation = mSupportedOperations.find(operation);
  if (operationLocation != std::string::npos) {
    return mOperationPriority[operation];
  }
  else {
    return -1;
  }
}


const std::string
XdmfFunction::getSupportedOperations()
{
        return mSupportedOperations;
}

const std::vector<std::string>
XdmfFunction::getSupportedFunctions()
{
  std::vector<std::string> returnVector;
  for (std::map<std::string, shared_ptr<XdmfArray>(*)(std::vector<shared_ptr<XdmfArray> >)>::iterator functionWalker
       = arrayFunctions.begin();
       functionWalker != arrayFunctions.end();
       ++functionWalker) {
    returnVector.push_back(functionWalker->first);
  }
  return returnVector;
}

const std::string
XdmfFunction::getValidDigitChars()
{
        return mValidDigitChars;
}

const std::string
XdmfFunction::getValidVariableChars()
{
        return mValidVariableChars;
}

shared_ptr<XdmfArray>
XdmfFunction::interlace(shared_ptr<XdmfArray> val1, shared_ptr<XdmfArray> val2)
{
  // Join interlace (evenly space the second array within the first one)
  // Builds a new array
  shared_ptr<XdmfArray> returnArray = XdmfArray::New();
  // Resize to the combined size of both arrays
  // Determining what type to class it as in order to not lose data, and to still have the smallest data type of the two
  shared_ptr<const XdmfArrayType> resultType = XdmfArrayType::comparePrecision(val1->getArrayType(), val2->getArrayType());

  if (resultType == XdmfArrayType::Int8()) {
    char sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Int16()) {
    short sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Int32()) {
    int sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Int64()) {
    long sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::UInt8()) {
    unsigned char sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::UInt16()) {
    unsigned short sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::UInt32()) {
    unsigned int sampleValue = 0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Float32()) {
    float sampleValue = 0.0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::Float64()) {
    double sampleValue = 0.0;
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else if (resultType == XdmfArrayType::String()) {
    std::string sampleValue = "";
    returnArray->resize(val1->getSize()+val2->getSize(), sampleValue);
  }
  else {
    // error type not supported
    XdmfError::message(XdmfError::FATAL, "Invalid type during Interlace");
  }

  // Determine ratio of array sizes
  int arrayRatio1 = (int)floor(static_cast<double>(val1->getSize())/val2->getSize());
  int arrayRatio2 = (int)floor(static_cast<double>(val2->getSize())/val1->getSize());
  if (arrayRatio1 < 1) {
    arrayRatio1 = 1;
  }
  if (arrayRatio2 < 1) {
    arrayRatio2 = 1;
  }
  // Stride is equal to the ratios rounded up and added together
  int stride = arrayRatio1+arrayRatio2;
  int arrayExcess1 = 0;
  int arrayExcess2 = 0;
  for (int i = 0; i < stride; ++i) {
    // Add the values of each array, using strides to interlace and starting index to offset
    // first array gets the first value of the new array
    if (i<arrayRatio1) {
      int amountWritten = val1->getSize()/arrayRatio1;
      if (((amountWritten * arrayRatio1) + i) < (int)val1->getSize()) {
        amountWritten++;
      }
      if (amountWritten > floor(val2->getSize()/arrayRatio2)) {
        arrayExcess1 += amountWritten - (int)floor(val2->getSize()/arrayRatio2);
        amountWritten = (int)floor(val2->getSize()/arrayRatio2);
      }
      returnArray->insert(i, val1, i, amountWritten, stride, arrayRatio1);
    }
    else {
      // Second array takes the rest
      int amountWritten = val2->getSize()/arrayRatio2;
      if (((amountWritten * arrayRatio2) + i) < (int)val2->getSize()) {
        amountWritten++;
      }
      if (amountWritten > floor(val1->getSize()/arrayRatio1)) {
        arrayExcess2 += amountWritten - (int)floor(val1->getSize()/arrayRatio1);
        amountWritten = (int)floor(val1->getSize()/arrayRatio1);
      }
      returnArray->insert(i, val2, i-arrayRatio1, amountWritten, stride, arrayRatio2);
    }
  }
  if (arrayExcess1 > 0) {
    returnArray->insert(val1->getSize()+val2->getSize()-arrayExcess1, val1, 0, arrayExcess1, 1, 1);
  }
  else if (arrayExcess2 > 0) {
    returnArray->insert(val1->getSize()+val2->getSize()-arrayExcess2, val2, 0, arrayExcess2, 1, 1);
  }
  // After all inserts are done, add the excess values to the end of the array
  return returnArray;
}

shared_ptr<XdmfArray>
XdmfFunction::sum(std::vector<shared_ptr<XdmfArray> > values)
{
  double total = 0.0;
  for (unsigned int i = 0; i < values.size(); ++i) {
    for (unsigned int j = 0; j < values[i]->getSize(); ++j) {
      total += values[i]->getValue<double>(j);
    }
  }
  shared_ptr<XdmfArray> returnArray = XdmfArray::New();
  returnArray->insert(0, total);
  return returnArray;
}

