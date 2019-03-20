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

#ifndef PSN_ENCODER_HPP
#define PSN_ENCODER_HPP

#include <psn_generic.hpp>
#include <list>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace psn
{

class psn_encoder
{
  public :
	psn_encoder( const ::std::string & system_name = "" ) ;
	~psn_encoder( void ) ;
	
	void set_trackers( const ::psn::psn_tracker_array & trackers ) ;

	uint8_t get_last_encode_info_packet_frame_id( void ) const { return info_packet_frame_id_ ; }
	uint8_t get_last_encode_data_packet_frame_id( void ) const { return data_packet_frame_id_ ; }

	bool encode_info( ::std::list< ::std::string > & encoded_info_packets , uint64_t timestamp = 0 ) ;
	bool encode_data( ::std::list< ::std::string > & encoded_data_packets , uint64_t timestamp = 0 ) ;

  private:
    
	psn_chunk		* psn_data_packet_ ;
	psn_chunk		* psn_data_packet_header_ ;
	psn_chunk		* psn_data_tracker_list_ ;

	psn_chunk		* psn_info_packet_ ;
	psn_chunk		* psn_info_packet_header_ ;
	psn_chunk		* psn_info_system_name_ ;
	psn_chunk		* psn_info_tracker_list_ ;

  private :

	::psn::psn_tracker_array	trackers_ ;

	uint8_t						info_packet_frame_id_ ;
	uint8_t						data_packet_frame_id_ ;
} ;

} // namespace psn

#endif
