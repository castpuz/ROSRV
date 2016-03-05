// this file modified by Morgan Quigley on 22 Apr 2008.
// added features: server can be opened on port 0 and you can read back
// what port the OS gave you

#ifndef RVCPP_XMLRPCSERVER_H_
#define RVCPP_XMLRPCSERVER_H_
//
// XmlRpc++ Copyright (c) 2002-2003 by Chris Morley
//
#if defined(_MSC_VER)
# pragma warning(disable:4786)    // identifier was truncated in debug info
#endif

#ifndef MAKEDEPEND
# include <map>
# include <string>
#endif

#include "rv/XmlRpcDispatch.h"
#include "rv/XmlRpcSource.h"
#include "XmlRpcDecl.h"
#include "XmlRpcValue.h"
#include "rv/callInfo.h"

namespace rv {


  // An abstract class supporting XML RPC methods
  class XmlRpcServerMethod2;

  // Class representing connections to specific clients
  class XmlRpcServerConnection;


  //! A class to handle XML RPC requests
  class XMLRPCPP_DECL XmlRpcServer : public XmlRpcSource {
  public:
    //! Create a server object.
    XmlRpcServer();
    //! Destructor.
    virtual ~XmlRpcServer();

    //! Specify whether introspection is enabled or not. Default is not enabled.
    void enableIntrospection(bool enabled=true);

    //! Add a command to the RPC server
    void addMethod(XmlRpcServerMethod2* method);

    //! Remove a command from the RPC server
    void removeMethod(XmlRpcServerMethod2* method);

    //! Remove a command from the RPC server by name
    void removeMethod(const std::string& methodName);

    //! Look up a method by name
    XmlRpcServerMethod2* findMethod(const std::string& name) const;

    //! Create a socket, bind to the specified port, and
    //! set it in listen mode to make it available for clients.
    bool bindAndListen(int port, int backlog = 5);

    //! Process client requests for the specified time
    void work(double msTime);

    //! Temporarily stop processing client requests and exit the work() method.
    void exit();

    //! Close all connections with clients and the socket file descriptor
    void shutdown();

    //! Introspection support
    void listMethods(XmlRpc::XmlRpcValue& result);

    // XmlRpcSource interface implementation

    //! Handle client connection requests
    virtual unsigned handleEvent(unsigned eventType);

    //! Remove a connection from the dispatcher
    virtual void removeConnection(XmlRpcServerConnection*);

    inline int get_port() { return _port; }
    
    rv::ClientInfo findClientInfo(int fd, rv::ClientInfo &ci) {
      ci.ip=map_ip[fd];
      ci.port=map_port[fd];
      ci.family=map_family[fd];
     return ci;
     }

    XmlRpcDispatch *get_dispatch() { return &_disp; }

  protected:

    //! Accept a client connection request
    virtual void acceptConnection();

    //! Create a new connection object for processing requests from a specific client.
    virtual XmlRpcServerConnection* createConnection(int socket);

    // Whether the introspection API is supported by this server
    bool _introspectionEnabled;

    // Event dispatcher
    XmlRpcDispatch _disp;

    // Collection of methods. This could be a set keyed on method name if we wanted...
    typedef std::map< std::string, XmlRpcServerMethod2* > MethodMap;
    MethodMap _methods;

    // system methods
    XmlRpcServerMethod2* _listMethods;
    XmlRpcServerMethod2* _methodHelp;

    int _port;
    rv::ClientInfo sci;
//  std::map<int,rv::ClientInfo> map_ci;  
  std::map<int,std::string> map_ip;
  std::map<int,short> map_family;
  std::map<int,int> map_port;
  };
} // namespace XmlRpc

#endif //_XMLRPCSERVER_H_

