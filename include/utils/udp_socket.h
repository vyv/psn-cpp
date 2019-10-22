//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/*
The MIT License (MIT)

Copyright (c) 2014 VYV Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include <WinSock2.h>
#include <Ws2tcpip.h>

#pragma message("Note: including lib: Ws2_32.lib") 
#pragma comment(lib, "Ws2_32.lib")

#include <string>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class wsa_session
{
  public :

    wsa_session()
    {
        WSAStartup( MAKEWORD( 2 , 2 ) , &data_ ) ;
    }
    ~wsa_session()
    {
        WSACleanup() ;
    }

  private : 

    WSAData data_ ;
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class udp_socket
{
  public :

	udp_socket( void ) 
	{
		// UDP socket
		socket_ = socket( AF_INET , SOCK_DGRAM , IPPROTO_UDP ) ;

		// non blocking socket
		u_long arg = 1 ;
		ioctlsocket( socket_ , FIONBIO , &arg ) ;
	}

	~udp_socket( void ) 
	{
		closesocket( socket_ ) ;
	}

	bool send_message( const std::string & address , unsigned short port , const ::std::string & message )
    {
        sockaddr_in add ;
        add.sin_family = AF_INET ;
        InetPton( AF_INET , address.c_str() , &add.sin_addr.s_addr ) ;
        add.sin_port = htons( port ) ;
        
		if ( sendto( socket_ , message.c_str() , (int)message.length() , 0 , reinterpret_cast<SOCKADDR *>( &add ) , sizeof( add ) ) > 0 )
			return true ;

		return false ;
    }

    bool receive_message( ::std::string & message , int max_size = 1500 )
    {
		if ( max_size <= 0 )
			return false ;

		char * buffer = (char *)malloc( max_size ) ;

        int byte_recv = recv( socket_ , buffer , max_size , 0 ) ;

		if ( byte_recv > 0 )
			message = ::std::string( buffer , byte_recv ) ;

		free( buffer ) ;

		return ( byte_recv > 0 ) ;
    }

    bool bind( unsigned short port )
    {
        sockaddr_in add ;
        add.sin_family = AF_INET ;
        add.sin_addr.s_addr = htonl( INADDR_ANY ) ;
        add.sin_port = htons( port ) ;

		if ( ::bind( socket_ , reinterpret_cast<SOCKADDR *>( &add ) , sizeof( add ) ) != SOCKET_ERROR )
			return true ;

		return false ;
    }

	bool enable_send_message_multicast( void ) 
	{
		struct in_addr add ;
		add.s_addr = INADDR_ANY ;

		int result = setsockopt( socket_ , 
						   IPPROTO_IP , 
						   IP_MULTICAST_IF , 
						   (const char*)&add , 
						   sizeof( add ) ) ;
		return ( result != SOCKET_ERROR ) ; 
	}

	bool join_multicast_group( const ::std::string & ip_group ) 
	{
		struct ip_mreq imr; 
        
        InetPton( AF_INET , ip_group.c_str() , &imr.imr_multiaddr.s_addr ) ;
		imr.imr_interface.s_addr = INADDR_ANY ;

		int result = setsockopt( socket_ , 
						   IPPROTO_IP , 
						   IP_ADD_MEMBERSHIP , 
						   (char*) &imr , 
						   sizeof(struct ip_mreq) ) ;
		return ( result != SOCKET_ERROR ) ; 
	}

	bool leave_multicast_group( const ::std::string & ip_group ) 
	{
		struct ip_mreq imr;
        
        InetPton( AF_INET , ip_group.c_str() , &imr.imr_multiaddr.s_addr ) ;
		imr.imr_interface.s_addr = INADDR_ANY ;

		int result = setsockopt( socket_ , 
						   IPPROTO_IP , 
						   IP_DROP_MEMBERSHIP ,
						   (char*) &imr , 
						   sizeof(struct ip_mreq) ) ;
		return ( result != SOCKET_ERROR ) ; 
	}

  private :

	SOCKET		socket_ ; 
} ;

#endif
