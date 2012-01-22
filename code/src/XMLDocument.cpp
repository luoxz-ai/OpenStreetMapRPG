/**
 * \file XMLDocument.cpp
 * \brief Contains class implementation of XMLDocument
 *
 * Modifications:
 * - <b>2005-12-23</b>: Changed the code of the state related methods, bacause State has changed
 * - <b>2005-06-15</b>: Created
 * */ 

#include "XMLDocument.h"

//!@name Constructors and destructors
//@{

//! Default constructor.
/**
 * Opens the xml file whose name is given by filename.
 * 
 * Possible exception:
 * - EOpenFailed : the parsing of the document failed.
 * - EEmpyDocument : the document does not have an root element
 * */
XMLDocument::XMLDocument(const std::string& filename)
{ 
  m_doc = xmlParseFile(filename.c_str());
  if (m_doc==0)
  {
    EOpenFailed error;
    error.SetFunction("XMLDocument::XMLDocument()");
    error.SetMessage("Failed to open the file " + filename + ".");
    throw error;
  }
  m_cur = xmlDocGetRootElement(m_doc);
  if (m_cur==0) 
  {
    xmlFreeDoc(m_doc);
    EEmptyDocument error;
    error.SetFunction("XMLDocument::XMLDocument()");
    error.SetMessage("The file " + filename + " is not an valid xml file.");
    throw error;
  }
  m_reset = true;
}
  
//! Destructor.
XMLDocument::~XMLDocument()
{ 
  if (m_doc) xmlFreeDoc(m_doc);
}
//@}

//!@name Getting values
//@{

//! Gets the name of the current node.
/**
 * For example, returns "foo" for the node <foo prop="bar">flierp</foo>
 * */
std::string XMLDocument::GetName()
{
  return (char *)m_cur->name;
}

//! Gets the value of the property.
/**
 * For example, returns "bar" for the node <foo prop="bar">flierp</foo> when called as GetProperty("prop").
 * Returns an empty string when the property does not exist.
 * */
std::string XMLDocument::GetProperty(const std::string& property)
{
  xmlChar*    key = xmlGetProp(m_cur,(const xmlChar*)property.c_str());
  std::string str = key ? (char *)key : "";
#ifdef WIN32
  xmlFreeFunc((void*)key);
#else
  xmlFree(key);
#endif
  return str;
}

//! Checks if the current node has a certain property.
/**
 * For example, returns true for the node <foo prop="bar">flierp</foo> when called as HasProperty("prop").
 * */
bool XMLDocument::HasProperty(const std::string& property)
{
  xmlChar* key = xmlGetProp(m_cur,(const xmlChar*)property.c_str());
  bool     prop = key ? true : false;
#ifdef WIN32
  xmlFreeFunc((void*)key);
#else
  xmlFree(key);
#endif
  return prop;
}

//! Gets the content of the node.
/**
 * For example, returns "flierp" for the node <foo prop="bar">flierp</foo>.
 * */
std::string XMLDocument::GetValue()
{
  xmlChar*    key = xmlNodeListGetString(m_doc, m_cur->xmlChildrenNode, 1);
  std::string str = key ? (char *)key : "";
#ifdef WIN32
  xmlFreeFunc((void*)key);
#else
  xmlFree(key);
#endif
  return str;
}
//@}

//!@name Navigation routines
//@{
//! Returns to the root node.
void XMLDocument::Reset()
{
  m_cur = xmlDocGetRootElement(m_doc);
  m_reset = true;
}

//! Goes to the first daughter of the current node.
/**
 * Returns false when the node does not contain a daughter.
 * */
bool XMLDocument::Enter()
{
  if (m_cur->xmlChildrenNode!=0)
  {
    m_cur   = m_cur->xmlChildrenNode;
    m_reset = true;
    return true;
  }
  else return false;
}

//! Goes to the mother of the current node.
void XMLDocument::Leave()
{
  m_cur   = m_cur->parent;
  m_reset = false;
}

//! Goes to the next node.
/**
 * Returns false when there is no more node, true otherwise. The method can therefore be used
 * in a while loop:
 * \code
 * while (xmldoc.Next()) 
 * {  
 *   // do something with the node
 * }
 * \endcode
 * */
bool XMLDocument::Next()
{ 
  if (m_cur->next!=0) 
  { 
    m_cur   = m_cur->next;
    m_reset = false;
    return true;
  }
  else 
  {
    m_reset = false;
    return false;
  }
}

//! Goes to the previous node.
/**
 * Returns false when there is no more node, true otherwise. The method can therefore be used
 * in a while loop:
 * \code
 * while (xmldoc.Previous()) 
 * { 
 *  // do something with the node 
 * }
 * \endcode
 * */
bool XMLDocument::Previous()
{
  if (m_cur->prev!=0)
  {
    m_cur = m_cur->prev;
    return true;
  }
  else
  {
    m_reset = true;
    return false;
  }
}

//! Goes to the first node.
void XMLDocument::First()
{
  while (Previous()) ;
  m_reset = true;
}
//@}

//!@name Finding nodes
//@{ 

//! Finds the next node the name given by 'tag'.
/**
 * Returns true when found false otherwise.
 * Find only looks at the current depth.
 * */
bool XMLDocument::Find(const std::string& tag)
{ 
  if (!m_reset) 
  { if (!Next()) return false;}
  
  do
  { 
    if (GetName()==tag) 
    {
      m_reset = false;
      return true;
    }
  }
  while (Next());
  
  return false;
}

//! Finds the next node the name given by 'tag' and the property name equal to 'value'.
/**
 * Returns true when found false otherwise.
 * Find only looks at the current depth.
 * */
bool XMLDocument::Find(const std::string& tag, const std::string& name)
{
  return Find(tag,"name",name);
}

//! Finds the next node the name given by 'tag' and the property 'property' equal to 'value'.
/**
 * Returns true when found false otherwise.
 * Find only looks at the current depth.
 * */
bool XMLDocument::Find(const std::string& tag, const std::string& property, const std::string& value)
{ 
  if (!m_reset) 
  { if (!Next()) return false;}
  
  do
  { 
    if (GetName()==tag)
    { 
      if (GetProperty(property)==value) 
      {
	m_reset = false;
	return true;
      }
    }
  }
  while (Next());
  
  return false;
}
//@}

//! Gets the current position in the document.
/**
 * Can be used in combination with SetState() to return to the previous position in the document.
 * */
XMLDocument::State XMLDocument::GetState()
{ return XMLDocument::State(m_cur, m_reset);}

//! Sets the position in the document back to the one given by state.
void XMLDocument::SetState(XMLDocument::State state)
{
  m_cur   = state.GetCurrentNode();
  m_reset = state.IsReset();
}
    

