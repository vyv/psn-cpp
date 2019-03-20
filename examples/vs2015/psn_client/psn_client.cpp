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

#include <psn_lib.hpp>

#include <utils/udp_socket.h>

#include <iostream>

void main( void )
{
	wsa_session session ;

//====================================================
// Init client
	udp_socket socket_client ;
	socket_client.bind( PSN_DEFAULT_UDP_PORT ) ;
	socket_client.join_multicast_group( PSN_DEFAULT_UDP_MULTICAST_ADDR ) ;

	::psn::psn_decoder psn_decoder ;

//====================================================
// Main loop
	while ( 1 ) 
	{
		Sleep( 1 ) ;

		// Update Client
		{
			::std::string msg ;
			if ( socket_client.receive_message( msg , PSN_MAX_UDP_PACKET_SIZE ) )
			{
				psn_decoder.decode( msg ) ;

				static uint8_t last_frame_id = 0 ;

				if ( psn_decoder.get_last_decoded_frame_id() != last_frame_id )
				{
					last_frame_id = psn_decoder.get_last_decoded_frame_id() ;

					::psn::psn_tracker_array recv_trackers = psn_decoder.get_trackers() ;
					::psn::psn_tracker_array::iterator it ;
				
					static int skip_cout = 0 ;
					if ( skip_cout++ % 100 == 0 )
					{
						static ::std::string server_name ;
						psn_decoder.get_psn_server_name( server_name ) ;

						::std::cout << "--------------------PSN CLIENT-----------------" << ::std::endl ;
						::std::cout << "PSN Server : " << server_name << ::std::endl ;
						::std::cout << "Frame Id : " << (int)last_frame_id << ::std::endl ;
						::std::cout << "Trackers : " << ::std::endl ;

						for ( it = recv_trackers.begin() ; it != recv_trackers.end() ; ++it )
						{
							::std::cout << it->second.id_ << " \""
										<< it->second.name_ << "\" "
										<< it->second.pos_.x << " , "
										<< it->second.pos_.y << " , "
										<< it->second.pos_.z << " | "
										<< it->second.ori_.x << " , "
										<< it->second.ori_.y << " , "
										<< it->second.ori_.z << ::std::endl ;
						}

						::std::cout << "-----------------------------------------------" << ::std::endl ;
					}
				}
			}
		}
	}
}


