// JigLib - Copyright (C) 2004 Danny Chapman
#ifndef JIGCONFIGFILE_H
#define JIGCONFIGFILE_H

#include "../include/jiglibconfig.hpp"
#include "../utils/include/trace.hpp"
#include "../utils/include/assert.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <vector>

//! Utility class for opening and parsing text config files
/*!
  
Note that there are two ways of parsing configuration files:
<ol>
<li> Specifying an attribute that you suspect exists in the config
file, and obtaining the value(s) associated with it.
<li> Asking for the next attribute/value pair in the file.
</ol>
It is possible to mix-and-match the two methods with this class.

The comment character in the config file is assumed to be "#".
*/
namespace JigLib
{
  class tConfigFile
  {
  public:
    /// success indicate ability to open file_name
    tConfigFile(const std::string & file_name, bool & success);
    /// Copy constructor
    tConfigFile(const tConfigFile & orig);
    /// Equals operator
    tConfigFile operator=(const tConfigFile & orig);
    
    /// Destructor closes any open file
    ~tConfigFile();
    
    /// Indicates the config file name
    std::string get_file_name() const {return m_file_name;}
    
    /// This returns the value for the first occurence of attr in the
    /// file. It does not affect the location used in the get_next
    /// functions.  
    ///
    /// If the get fails, the value (passed by reference) is
    /// unmodified.
    ///
    /// \return true if successful. False if either the attribute
    /// isn't found, or it has no value associated with it. In the
    /// case that multiple values are found, only the first is used
    /// (an error message is generated).
    template<class T>
    bool GetValue(const std::string & attr, T & value)
      {
        std::vector<T> values;
        bool retval = GetValues(attr, values);
        if (false == retval)
        {
          return false;
        }
        if (values.size() == 0)
        {
          TRACE_FUNCTION(); TRACE("No values!\n");
          return false;
        }
        if (values.size() > 1)
        {
          TRACE_FILE_IF(1) 
            {
              TRACE_FUNCTION(); TRACE("Multiple values for attribute %s\n", attr.c_str());
            }
        }
        value = values[0];
        return true;
      }
    
    
    /// Version that asserts on failure: same as GetValue, except
    /// asserts in case of failure
    template<class T>
    void GetValueAssert(const std::string & attr, T & value)
      {
        bool retval = GetValue(attr, value);
        if (retval != true)
        {
          TRACE("Failed to find attribute %s\n", attr.c_str());
          Assert(!"Error!");
        }
      }
    
    /// Get multiple values for an attribute
    ///
    /// This returns the values for the first occurence of attr in the
    /// file. It does not affect the location used in the get_next
    /// functions.
    /// 
    /// If the get fails, the values (passed by reference) is unmodified. 
    /// 
    /// \return true if successful. False if the attribute is not
    /// found, or if it has no values associated with it.*/
    template<class T>
    bool GetValues(const std::string & attr, std::vector<T> & values)
      {
        // reset the random access stream
        Reset(m_rand_file_stream);
        
        std::vector<std::string> words;
        
        while (true == GetNextProcessedLine(m_rand_file_stream, words))
        {
          if ( (words.size() > 0) && (words[0] == attr) )
          {
            // found it!
            values.resize(0);
            if (1 == words.size())
            {
              TRACE("Found attribute %s but no value\n", attr.c_str());
              return false;
            }
            // we don't actually know the type of value, so use its ability
            // to read in from a stream.
            unsigned int i;
            std::string everything;
            for (i = 1 ; i < words.size() ; ++i)
            {
              everything += words[i];
              everything += " ";
            }
            std::istringstream is(everything);
            
            T value;
//      cout << "Read: " << attr;
            for (i = 1 ; i < words.size() ; ++i)
            {
              is >> value;
              values.push_back(value);
//        cout << value << " ";
            }
//      cout << endl;
            return true;
          }
        }
        // didn't find what we were looking for
        return false;
      }
    
    /// Contains the details of an attr/value configuration entry. Needs
    /// to be able to cope with multiple values, of an unknown type.
    struct tConfigAttrValue
    {
      tConfigAttrValue() : attr("None"), value_type(INVALID), num(0) {};
      std::string attr; ///< The attribute name
      /// INVALID is used if nothing found
      enum tValueType {INVALID, BOOL, FLOAT, STRING} value_type;
      unsigned int num; ///< Number of values - 0 if not found
      struct tValue
      {
        bool   bool_val;
        float  float_val;
        std::string string_val;
      };
      std::vector<tValue> values;
    };
    /// Returns the next attr/value entry. In the case of EOF, sets the
    /// type = INVALID, and num = 0
    tConfigAttrValue GetNextAttrValue();
    
    /// Returns the next line of values as a vector (i.e. as in
    /// tConfigAttrValue but with no attr).
    template<class T>
    bool GetNextValues(std::vector<T> & values, int num = 0)
      {
        values.resize(0);
        std::vector<std::string> words;
        
        if (false == GetNextProcessedLine(m_seq_file_stream, words))
        {
          // reached end of file
          return false;
        }          
        T value;
        if (num > 0)
        {
          if ((int) words.size() < num)
          {
            TRACE("expected %d values, found %d\n", num, words.size());
            TRACE("values read in were:\n");
            unsigned i;
            for (i = 0 ; i < words.size() ; ++i)
            {
              TRACE("%s\n", words[i].c_str());
            }
            Assert(!"Error!")
              }
          else if ((int) words.size() > num)
          {
            TRACE("expected %d values, found %d so ignoring the extras\n", words.size(), num);
            words.resize(num);
          }
        }
        for (unsigned i = 0 ; i < words.size() ; ++i)
        {
          std::istringstream is(words[i]);
          is >> value;
          values.push_back(value);
        }
        return true;
      }
    
    
    /// Gets the next value, asserting that it is of the specified
    /// attribute
    template<class T>
    void GetNextValueAssert(const std::string & attr, T & value)
      {
        std::vector<std::string> words;
        
        while (true == GetNextProcessedLine(m_seq_file_stream, words))
        {
          if (words.size() < 2)
          {
            TRACE("Expecting at least two words with attribute %s\n",
                  attr.c_str());
            Assert(!"Error!");
          }
          if (words[0] != attr)
          {
            TRACE("Expected attribute %s : found %s\n", 
                  attr.c_str(), words[0].c_str());
            Assert(!"Error!");
          }
          
          // we don't actually know the type of value, so use its ability
          // to read in from a stream.
          std::istringstream is(words[1]);
//    cout << "Attribute: " << attr;
          is >> value;
//    cout << value << endl;
          return;
        }
        TRACE("Attribute %s not found\n", attr.c_str());
        Assert(!"Error!");
      }
    
    
    /// Gets the next value, WITHOUT asserting that it is of the
    /// specified attribute. value is unchanged if the attr is not as
    /// requested, and the read position does not advance (except for
    /// white-space).  the return value indicates if the read was
    /// successful.
    template<class T>
    bool GetNextValue(const std::string & attr, T & value)
      {
        std::vector<std::string> words;
        // store position
        
        // Apparantly std::streampos should really be std::ios::pos_type, but
        // maybe my compiler is a bit old-fashioned?
        std::streampos pos = m_seq_file_stream->tellg();
        
        while (true == GetNextProcessedLine(m_seq_file_stream, words))
        {
          Assert(words.size() >= 2);
          if (words[0] == attr)
          {
            // we don't actually know the type of value, so use its ability
            // to read in from a stream.
            std::istringstream is(words[1]);
//      cout << "Attribute: " << attr;
            is >> value;
//      cout << value << endl;
            return true;
          }
          else
          {
            // we have to rewind
            m_seq_file_stream->seekg(pos);
            return false;
          }
        }
        TRACE("Attribute %s not found\n", attr.c_str());
        Assert(!"Error!");
        return false;
      }
    
    /// finds the next block starting with "begin block_name". Returns
    /// true if found, false if not.
    bool FindNewBlock(const std::string & block_name);
    
    /// Finds the next occurence of the desired attribute, and returns
    /// the attr/value result
    tConfigAttrValue FindNextAttrValue(const std::string & attr);
    
    ///  resets the internal sequential file descriptor to point to
    ///  the beginning of the file - so the next call to
    ///  get_next_attr_value will return the first.
    void Reset() {Reset(m_seq_file_stream);}
    
    /// Returns the stream pointer used in get_next_attr_value(...).
    std::ifstream * GetStream() const {return m_seq_file_stream;}
    
  private:
    /// Seeks the specified stream to the beginning of file
    void Reset(std::ifstream * & stream);
    
    /// gets a vector of words for the next line. Returns false at EOF
    bool GetNextProcessedLine(
      std::ifstream * stream, std::vector<std::string> & words);
    
    const std::string m_file_name;
    std::ifstream * m_seq_file_stream;  ///< stream used for sequential access
    std::ifstream * m_rand_file_stream; ///< stream used for random access
    
    // some static consts
    static const std::string comment_char; ///< Comment character
    static const std::string delims;       ///< "word" delimiters
    static const std::string digits;       ///< used to identify numbers
  };
  
}

#endif  


