//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 VYV Corporation
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
**/
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#include <psn_lib.hpp>
#include <kissnet.hpp>

#include <iostream>

#include <string.h>
#include <unistd.h>
#include <chrono>
#include <thread>

const uint16_t           PORT         = 56565;
static const short    BUFLEN       = 1024;
static const uint32_t TIMEOUT_MSEC = 1000;

namespace kn = kissnet;

int main( void )
{
     char buf[BUFLEN];
    // wsa_session session ;


//====================================================
// Init client
    //Socket used to receive, the "endpoint" is where to listen to data
    kn::udp_socket b_socket(kn::endpoint("0.0.0.0", 56565));
    b_socket.set_multicast();
    b_socket.bind();

    // UdpServerSocket server(PORT, TIMEOUT_MSEC);
    // udp_socket socket_client ;
    // socket_client.bind( ::psn::DEFAULT_UDP_PORT ) ;
    // socket_client.join_multicast_group( ::psn::DEFAULT_UDP_MULTICAST_ADDR ) ;

    ::psn::psn_decoder psn_decoder ;
    uint8_t last_frame_id = 0 ;
    int skip_cout = 0 ;

    //====================================================
    // Main loop
    while ( 1 ) 
    {
        // Sleep( 1 ) ;
        // sleep(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(1) );



        printf("Waiting for data...");
        fflush(stdout);

        // memset(buf, 0, BUFLEN);

        // *buf = 0;

        // server.receiveData(buf, BUFLEN);
        kn::buffer<1024> recv_buff;

        //Actually print bytes_available
		std::cout << "available in UDP socket : " << b_socket.bytes_available() << " bytes\n";

		//You receive in the same way
		auto [received_bytes, status] = b_socket.recv(recv_buff);
		const auto from = b_socket.get_recv_endpoint();

		//Print the data
		std::cout << "Received: ";

		for(unsigned char i = 0; i < 16; i++)
		{
			std::cout << std::hex << std::to_integer<int>(recv_buff[i]) << std::dec << ' ';
		}

		//Print who send the data
		std::cout << "From: " << from.address << ' ' << from.port << '\n';

        // if (recv_buff) {
        //     printf("Data: %s\n", buf);
        //     // server.sendData(buf, strlen(buf));

        //     // std::cout << "As string: " << msg << " with size " << msg.size() << std::endl;
            
        //     psn_decoder.decode( buf , BUFLEN ) ;

        //     // if ( psn_decoder.get_data().header.frame_id != last_frame_id )
        //     // {
        //         last_frame_id = psn_decoder.get_data().header.frame_id ;

        //         const ::psn::tracker_map & recv_trackers = psn_decoder.get_data().trackers ;
                
        //         // if ( skip_cout++ % 20 == 0 )
        //         // {
        //             ::std::cout << "--------------------PSN CLIENT-----------------" << ::std::endl ;
        //             ::std::cout << "System Name: " << psn_decoder.get_info().system_name << ::std::endl ;
        //             ::std::cout << "Frame Id: " << (int)last_frame_id << ::std::endl ;
        //             ::std::cout << "Frame Timestamp: " << psn_decoder.get_data().header.timestamp_usec << ::std::endl ;
        //             ::std::cout << "Tracker count: " << recv_trackers.size() << ::std::endl ;

        //             for ( auto it = recv_trackers.begin() ; it != recv_trackers.end() ; ++it )
        //             {
        //                 const ::psn::tracker & tracker = it->second ;

        //                 ::std::cout << "Tracker - id: " << tracker.get_id() << " | name: " << tracker.get_name() << ::std::endl ;

        //                 if ( tracker.is_pos_set() )
        //                     ::std::cout << "    pos: " << tracker.get_pos().x << ", " << 
        //                                                   tracker.get_pos().y << ", " <<
        //                                                   tracker.get_pos().z << std::endl ;

        //                 if ( tracker.is_speed_set() )
        //                     ::std::cout << "    speed: " << tracker.get_speed().x << ", " << 
        //                                                     tracker.get_speed().y << ", " <<
        //                                                     tracker.get_speed().z << std::endl ;

        //                 if ( tracker.is_ori_set() )
        //                     ::std::cout << "    ori: " << tracker.get_ori().x << ", " << 
        //                                                   tracker.get_ori().y << ", " <<
        //                                                   tracker.get_ori().z << std::endl ;

        //                 if ( tracker.is_status_set() )
        //                     ::std::cout << "    status: " << tracker.get_status() << std::endl ;

        //                 if ( tracker.is_accel_set() )
        //                     ::std::cout << "    accel: " << tracker.get_accel().x << ", " << 
        //                                                     tracker.get_accel().y << ", " <<
        //                                                     tracker.get_accel().z << std::endl ;

        //                 if ( tracker.is_target_pos_set() )
        //                     ::std::cout << "    target pos: " << tracker.get_target_pos().x << ", " << 
        //                                                          tracker.get_target_pos().y << ", " <<
        //                                                          tracker.get_target_pos().z << std::endl ;

        //                 if ( tracker.is_timestamp_set() )
        //                     ::std::cout << "    timestamp: " << tracker.get_timestamp() << std::endl ;
        //             }

        //             ::std::cout << "-----------------------------------------------" << ::std::endl ;
        //         //} // skip

        // }

        // else {
        //     printf("\n");
        //     // sleep(1);
        //     std::this_thread::sleep_for(std::chrono::milliseconds(1) );
        // }



        // if ( socket_client.receive_message( msg , ::psn::MAX_UDP_PACKET_SIZE ) )
        // {

        
        // }
        std::this_thread::sleep_for(std::chrono::milliseconds(1) );

    }

    return 0;
}


