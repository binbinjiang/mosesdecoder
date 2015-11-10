// -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-
#include "Server.h"
namespace MosesServer
{
  Server::
  Server(Moses::Parameter& params)
    : m_server_options(params),
      m_updater(new Updater),
      m_optimizer(new Optimizer),
      m_translator(new Translator(*this)),
      m_close_session(new CloseSession(*this))
  {
    m_registry.addMethod("translate", m_translator);
    m_registry.addMethod("updater",   m_updater);
    m_registry.addMethod("optimize",  m_optimizer);
    m_registry.addMethod("close_session", m_close_session);
  }

  int 
  Server::
  run()
  {
    xmlrpc_c::serverAbyss myAbyssServer
      (xmlrpc_c::serverAbyss::constrOpt()
       .registryP(&m_registry)
       .portNumber(m_server_options.port) // TCP port on which to listen
       .logFileName(m_server_options.logfile)
       .allowOrigin("*")
       .maxConn(m_server_options.num_threads));
    
    XVERBOSE(1,"Listening on port " << m_server_options.port << std::endl);
    if (m_server_options.is_serial) 
      {
        VERBOSE(1,"Running server in serial mode." << std::endl);
        while(true) myAbyssServer.runOnce();
      }
    else myAbyssServer.run();
    
    std::cerr << "xmlrpc_c::serverAbyss.run() returned but it should not." 
              << std::endl;
    return 1;
  }

  Moses::ServerOptions const& 
  Server::
  options() const
  {
    return m_server_options;
  }

  Session const& 
  Server::
  get_session(uint64_t session_id)
  {
    return m_session_cache[session_id];
  }

  void
  Server::
  delete_session(uint64_t const session_id)
  {
    return m_session_cache.erase(session_id);
  }
}
