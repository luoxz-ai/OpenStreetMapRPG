/**
 * \file Exception.cpp
 * \brief Contains class implementation of Exception
 *
 * Modifications:
 * - <b>2005-06-15</b>: Created
 * */ 

#include "Exception.h"

//!@name Constructors and destructors
//@{

//! Default constructor.
/**
 * \param[in] function : the function in which the exception occured
 * \param[in] message: a message describing the nature of the exception
 * */
Exception::Exception(const std::string& function, const std::string message) :
  m_function(function), m_message(message)
{}

//! Copy constructor.
Exception::Exception(const Exception& exception) :
  m_function(exception.GetFunction()), m_message(exception.GetMessage())
{}

//! Destructor
Exception::~Exception()
{}
//@}

//!@name Setting values
//@{

//! Set the name of the funtion in which the exception occured
void Exception::SetFunction(const std::string& function)
{ m_function = function;}

//! Set the message descibing the nature of the exception
void Exception::SetMessage(const std::string& message)
{ m_message = message;}
//@}

//!@name Getting values
//@{

//! Get the name of the funtion in which the exception occured
std::string Exception::GetFunction() const
{ return m_function;}
  
//! Get the message descibing the nature of the exception
std::string Exception::GetMessage() const
{ return m_message;}
//@}
    

//! Writes the error to the ostream.
/**
 * Can be used to output the error message to the std::cerr stream.
 * */
std::ostream& operator<<(std::ostream& stream, const Exception& exception)
{
  stream << "ERROR: in: " << exception.GetFunction() << "\n";
  stream << "       with message: " << exception.GetMessage() << "\n";
  return stream;
}
