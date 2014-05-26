#ifndef TEXCEPTION_HH
#define TEXCEPTION_HH

#include <string>

//using namespace std;

#if (__BCPLUSPLUS__ >= 0x0550)
#include <SysUtils.hpp>
class TException: public Exception {
public:
  TException(std::string msg) : Exception(msg.c_str()) {}
  TException() : Exception("") {}
  //TException( std::string msg = "" ) : Exception(msg.c_str()) {}
  virtual const char* what() const
	  {
	    return Message.c_str();
	  }

  virtual void what(const std::string& msg)
	  {
	    Message = msg.c_str();
	  }
};
#else
class TException {
    std::string Message;
public:
    TException( std::string msg = "" ) : Message(msg) {}
    virtual ~TException() {}
    virtual const char* what() const
	{
	    return Message.c_str();
	}

    virtual void what(std::string msg) 
	{
	    Message = msg;
	}
};
#endif



/*

class EOutOfMemory : public TException {
public:
    EOutOfMemory( string msg = "" ) : TException( "Error: out of memory" + msg ) {}
    virtual ~EOutOfMemory() {}
};
                 */
#endif
