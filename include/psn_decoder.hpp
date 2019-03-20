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

#ifndef PSN_DECODER_HPP
#define PSN_DECODER_HPP

#include <psn_generic.hpp>
#include <map>

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace psn
{

class psn_data_tracker_list_chunk ;
class psn_info_tracker_list_chunk ;

class psn_decoder
{
  public :

	psn_decoder( void ) ;
	~psn_decoder( void ) ;
	
	uint8_t get_last_decoded_frame_id( void ) const ;
	const psn_tracker_array & get_trackers( void ) const ;

	bool get_psn_server_name( ::std::string & server_name ) ;

	bool decode( const ::std::string & packet ) ;

  private : 

	bool decode_data_packet( void * buffer , size_t buffer_size ) ;
	bool decode_info_packet( void * buffer , size_t buffer_size ) ;

  private :
    
	psn_chunk						* psn_data_packet_ ;
	psn_chunk						* psn_data_packet_header_ ;
	psn_data_tracker_list_chunk		* psn_data_tracker_list_ ;

	psn_chunk						* psn_info_packet_ ;
	psn_chunk						* psn_info_packet_header_ ;
	psn_chunk						* psn_info_system_name_ ;
	psn_info_tracker_list_chunk		* psn_info_tracker_list_ ;

  private :

	psn_tracker_array						trackers_ ;
	psn_tracker_array						frame_trackers_ ; // temporary trackers until frame is completely decoded.

	::std::map< uint16_t , ::std::string >	trackers_info_ ; // keep the trackers info in a map

	uint8_t									last_frame_id_ ;
	uint8_t									frame_packet_counter_ ;

} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_data_tracker_list_chunk : public psn_chunk
{
  public :

	psn_data_tracker_list_chunk( uint16_t id , psn_chunk_data * data = 0 , const ::std::string & name = "" )
		: psn_chunk( id , data , name ) 
	{} 

	virtual ~psn_data_tracker_list_chunk( void )
	{}

  protected :

	// re-implement decode_child for PSN_DATA_TRACKER_LIST chunk
	// on decode_child, it will automatically add its own child
	virtual bool decode_child( uint16_t child_id , void * buffer , size_t buffer_size ) 
	{
		if ( get_child_by_id( child_id ) == 0 )
		{
			psn_chunk * psn_data_tracker = new psn_chunk( child_id , 0 , "PSN_DATA_TRACKER" ) ;

			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_POS	, new psn_vec3_data()			, "PSN_DATA_TRACKER_POS" ) ) ;
			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_SPEED	, new psn_vec3_data()			, "PSN_DATA_TRACKER_SPEED" ) ) ;
			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_ORI	, new psn_vec3_data()			, "PSN_DATA_TRACKER_ORI" ) ) ;
			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_STATUS , new psn_tracker_status_data()	, "PSN_DATA_TRACKER_STATUS" ) ) ;

			add_child( psn_data_tracker ) ;
		}

		return psn_chunk::decode_child( child_id , buffer , buffer_size ) ;
	}
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_info_tracker_list_chunk : public psn_chunk
{
  public :

	psn_info_tracker_list_chunk( uint16_t id , psn_chunk_data * data = 0 , const ::std::string & name = "" )
		: psn_chunk( id , data , name ) 
	{} 

	virtual ~psn_info_tracker_list_chunk( void )
	{}

  protected :

	// re-implement decode_child for PSN_INFO_TRACKER_LIST chunk
	// on decode_child, it will automatically add its own child
	virtual bool decode_child( uint16_t child_id , void * buffer , size_t buffer_size ) 
	{
		if ( get_child_by_id( child_id ) == 0 )
		{
			psn_chunk * psn_info_tracker = new psn_chunk( child_id , 0 , "PSN_INFO_TRACKER" ) ;

			psn_info_tracker->add_child( new psn_chunk( PSN_INFO_TRACKER_NAME , new psn_name_data() , "PSN_INFO_TRACKER_NAME" ) ) ;

			add_child( psn_info_tracker ) ;
		}

		return psn_chunk::decode_child( child_id , buffer , buffer_size ) ;
	}
} ;

} // namespace psn

#endif