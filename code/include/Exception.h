/**
 * \file Exception.h
 * \brief Contains class declaration of Exception
 *
 * Modifications:
 * - <b>2005-06-15</b>: Created
 * */ 

#ifndef Exception_h
#define Exception_h

#include <string>
#include <ostream>

//! General class for exceptions.
/**
 * All the exceptions in the program should inherit from this class.
 * */
class Exception
{
  public:
    Exception(const std::string& function="", const std::string message="");
    Exception(const Exception& exception);
    ~Exception();

    void SetFunction(const std::string& function);
    void SetMessage(const std::string& message);

    std::string GetFunction() const;
    std::string GetMessage() const;

    
  private:
    std::string m_function;
    std::string m_message;
};

std::ostream& operator<<(std::ostream& stream, const Exception& exception);

//! Returns an exception with the given function and message
/**
 * Can be used to make throwing an exception more easy:
 * \code
 * if (some error occured)
 *   throw GetException<SomeExceptionClass>("someclass::somemethod()", "Some error occured!");
 * \endcode
 * */
template<class C> inline C GetException(const std::string& function, const std::string& message)
{
  C error;
  error.SetFunction(function);
  error.SetMessage(message);
  return error;
};

#endif
