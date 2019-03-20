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
#include <math.h>

void main( void )
{
	wsa_session session ;

//====================================================
// Init server
	udp_socket socket_server ;
	socket_server.enable_send_message_multicast() ;

	::psn::psn_encoder psn_encoder( "Test PSN Server" ) ;

	::psn::psn_tracker_array trackers ;
	int i = 0 ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Sun" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Mercury" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Venus" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Earth" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Mars" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Jupiter" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Saturn" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Uranus" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Neptune" ) ;
	trackers[ i ] = ::psn::psn_tracker( i++ , "Pluto" ) ;

	// Planets orbit in days
	double orbits[ 10 ] = { 1. , 88. , 224.7 , 365.2 , 687.0 , 4332. , 10760. , 30700. , 60200. , 90600. } ;
	// Planets distance from sun in million km x 100
	double dist_from_sun[ 10 ] = { 0. , .58 , 1.08 , 1.50 , 2.28 , 7.78 , 14.29 , 28.71 , 45.04 , 59.13 } ;

//====================================================
// Main loop
	while ( 1 ) 
	{
		Sleep( 1 ) ;

		static uint64_t timestamp = 0 ;
		timestamp++ ;		

		// Update Server 
		{
			// Update trackers
			{
				static int day = 100000 ; 
				day++ ; // 1 day per millisecond

				for ( int i = 1 ; i < 10 ; ++i ) // do not update the sun
				{
					trackers[ i ].pos_.x = (float)sin( day / orbits[ i ] ) * (float)dist_from_sun[ i ] ;
					trackers[ i ].pos_.z = (float)cos( day / orbits[ i ] ) * (float)dist_from_sun[ i ] ;
				}
			}

			psn_encoder.set_trackers( trackers ) ;

			// Send data
			if ( timestamp % 15 == 0 ) // transmit data at 60 Hz approx.
			{
				::std::list< ::std::string > data_packets ;
		
				if ( psn_encoder.encode_data( data_packets , timestamp ) )
				{					
					::std::cout << "--------------------PSN SERVER-----------------" << ::std::endl ;
					::std::cout << "Sending PSN_DATA_PACKET : " 
							    << "Frame Id = " << (int)psn_encoder.get_last_encode_data_packet_frame_id()
								<< " , Packet Count = " << data_packets.size() << ::std::endl ;

					::std::list< ::std::string >::iterator it ;
					for ( it = data_packets.begin() ; it != data_packets.end() ; ++it )
					{
						// Uncomment these lines if you want to simulate a packet drop now and then
						/*static uint64_t packet_drop = 0 ;
						if ( packet_drop++ % 100 != 0 )*/
							socket_server.send_message( PSN_DEFAULT_UDP_MULTICAST_ADDR , PSN_DEFAULT_UDP_PORT , *it ) ;
					}

					::std::cout << "-----------------------------------------------" << ::std::endl ;
				}	
			}

			// Send Info
			if ( timestamp % 1000 == 0 ) // transmit info at 1 Hz approx.
			{
				::std::list< ::std::string > info_packets ;
		
				if ( psn_encoder.encode_info( info_packets , timestamp ) )
				{
					::std::cout << "--------------------PSN SERVER-----------------" << ::std::endl ;
					::std::cout << "Sending PSN_INFO_PACKET : " 
							    << "Frame Id = " << (int)psn_encoder.get_last_encode_info_packet_frame_id()
								<< " , Packet Count = " << info_packets.size() << ::std::endl ;

					::std::list< ::std::string >::iterator it ;			
					for ( it = info_packets.begin() ; it != info_packets.end() ; ++it )
						socket_server.send_message( PSN_DEFAULT_UDP_MULTICAST_ADDR , PSN_DEFAULT_UDP_PORT , *it ) ;

					::std::cout << "-----------------------------------------------" << ::std::endl ;
				}	
			}
		}
	}
}


