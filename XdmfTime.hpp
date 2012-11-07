/*****************************************************************************/
/*                                    XDMF                                   */
/*                       eXtensible Data Model and Format                    */
/*                                                                           */
/*  Id : XdmfTime.hpp                                                        */
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

#ifndef XDMFTIME_HPP_
#define XDMFTIME_HPP_

// Includes
#include "Xdmf.hpp"
#include "XdmfItem.hpp"

/**
 * @brief Time specification for an XdmfGrid.
 *
 * An XdmfTime sets a time value for an XdmfGrid.
 */
class XDMF_EXPORT XdmfTime : public XdmfItem {

public:

  /**
   * Create a new XdmfTime.
   *
   * Example of use:
   *
   * C++
   *
   * @code {.cpp}
   * //specifying a time
   * double newTime = 5.0;
   * shared_ptr<XdmfTime> exampleTime = XdmfTime::New(newTime);
   * //Or the default case can be used to have time set to 0
   * shared_ptr<XdmfTime> exampleTime2 = XdmfTime::New();
   * @endcode
   *
   * Python
   *
   * @code (.py}
   * '''
   * specifying a time
   * '''
   * newTime = 5.0
   * exampleTime = XdmfTime.New(newTime)
   * '''
   * Or the default case can be used to have time set to 0
   * '''
   * exampleTime2 = XdmfTime.New()
   * @endcode
   *
   * @param value the timeValue of the XdmfTime to create.
   * @return the new XdmfTime.
   */
  static shared_ptr<XdmfTime> New(const double & value = 0);

  virtual ~XdmfTime();

  LOKI_DEFINE_VISITABLE(XdmfTime, XdmfItem);
  static const std::string ItemTag;

  std::map<std::string, std::string> getItemProperties() const;

  std::string getItemTag() const;

  /**
   * Get the time value associated with this XdmfTime.
   *
   * Example of use:
   *
   * C++
   *
   * @code {.cpp}
   * //Assumming that exampleTime is a shared pointer to an XdmfTime object
   * double readTime = exampleTime->getTime();
   * @endcode
   *
   * Python
   *
   * @code {.py}
   * '''
   * Assumming that exampleTime is a shared pointer to an XdmfTime object
   * '''
   * readTime = exampleTime.getTime()
   * @endcode
   *
   * @return a double containing the time value.
   */
  double getValue() const;

  /**
   * Set the time value associated with this XdmfTime.
   *
   * Example of use:
   *
   * C++
   *
   * @code {.cpp}
   * shared_ptr<XdmfTime> exampleTime = XdmfTime::New();
   * double newTime = 5.0;
   * exampleTime->setValue(newTime);
   * //This sets the exampleTime's value to the value of newTime, which is 5.0.
   * @endcode
   *
   * Python
   *
   * @code {.py}
   * exampleTime = XdmfTime.New()
   * newTime = 5.0
   * exampleTime.setValue(newTime)
   * '''
   * This sets the exampleTime's value to the value of newTime, which is 5.0.
   * '''
   * @endcode
   *
   * @param time a double containing the time value.
   */
  void setValue(const double & time);

protected:

  XdmfTime(const double & value);

  virtual void
  populateItem(const std::map<std::string, std::string> & itemProperties,
               const std::vector<shared_ptr<XdmfItem> > & childItems,
               const XdmfCoreReader * const reader);

private:

  XdmfTime(const XdmfTime &);  // Not implemented.
  void operator=(const XdmfTime &);  // Not implemented.

  double mValue;
};

#endif /* XDMFTIME_HPP_ */
