/**
 * \file XMLDocument.h
 * \brief Contains class declaration of XMLDocument
 *
 * Modifications:
 * - <b>2005-12-23</b>: Made the State a subclass of XMLDocument instead of a struct outside XMLDocument
 * - <b>2005-06-15</b>: Created
 * */ 

#ifndef XMLDocument_hh
#define XMLDocument_hh

#include "Exception.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <string>

//! Class that allows the user to navigate through a xml document.
class XMLDocument
{
  public:
    //! Exception when the xml document could not be opened
    class EOpenFailed : public Exception {};
    //! exception when the document does not contain an root element
    class EEmptyDocument : public Exception {};
    //! Stores the state of the XMLDocument class.
    class State
    {
      public:
	State(xmlNodePtr cur, bool reset) : m_cur(cur), m_reset(reset) {};
	~State() {};
	xmlNodePtr GetCurrentNode() const {return m_cur;};
	bool IsReset() const {return m_reset;};
      private:
	xmlNodePtr m_cur;
	bool       m_reset;
    };
    
    XMLDocument(const std::string& filename);
    ~XMLDocument();
    
    std::string GetName();
    std::string GetProperty(const std::string& property);
    bool        HasProperty(const std::string& property);
    std::string GetValue();
    
    void Reset();
    bool Enter();
    void Leave();
    bool Next();
    bool Previous();
    void First();
    
    bool Find(const std::string& tag);
    bool Find(const std::string& tag, const std::string& name);
    bool Find(const std::string& tag, const std::string& property, const std::string& value);
    
    State GetState();
    void  SetState(State state);
      
  private:
    xmlDocPtr  m_doc;
    xmlNodePtr m_cur;
    bool       m_reset;
};

#endif
