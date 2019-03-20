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

#ifndef PSN_ENCODER_IMPL_HPP
#define PSN_ENCODER_IMPL_HPP

#include <psn_encoder.hpp> 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace psn
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_encoder::
psn_encoder( const ::std::string & system_name /*= ""*/ )
	: info_packet_frame_id_( 0 )
	, data_packet_frame_id_( 0 )
{
	// PSN_DATA 
	psn_data_packet_			= new psn_chunk( PSN_DATA_PACKET		, 0											, "PSN_DATA_PACKET" ) ;
	psn_data_packet_header_		= new psn_chunk( PSN_DATA_PACKET_HEADER	, new ::psn::psn_packet_header_data()		, "PSN_DATA_PACKET_HEADER" ) ;
	psn_data_tracker_list_		= new psn_chunk( PSN_DATA_TRACKER_LIST	, 0											, "PSN_DATA_TRACKER_LIST" ) ;
	// PSN_DATA_TRACKER chunks will be dynamically instantiated on encode_data 

	psn_data_packet_->add_child( psn_data_packet_header_ ) ;
	psn_data_packet_->add_child( psn_data_tracker_list_ ) ;

	// PSN_INFO
	psn_info_packet_			= new psn_chunk( PSN_INFO_PACKET		, 0											, "PSN_INFO_PACKET" ) ;
	psn_info_packet_header_		= new psn_chunk( PSN_INFO_PACKET_HEADER	, new ::psn::psn_packet_header_data()		, "PSN_INFO_PACKET_HEADER" ) ;
	psn_info_system_name_		= new psn_chunk( PSN_INFO_SYSTEM_NAME	, new ::psn::psn_name_data( system_name )	, "PSN_INFO_SYSTEM_NAME" ) ;
	psn_info_tracker_list_		= new psn_chunk( PSN_INFO_TRACKER_LIST	, 0											, "PSN_INFO_TRACKER_LIST" ) ;
	// PSN_INFO_TRACKER chunks will be dynamically instantiated on encode_info

	psn_info_packet_->add_child( psn_info_packet_header_ ) ;
	psn_info_packet_->add_child( psn_info_system_name_ ) ;
	psn_info_packet_->add_child( psn_info_tracker_list_ ) ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_encoder::
~psn_encoder( void ) 
{
	delete psn_data_packet_ ;
	psn_data_packet_ = 0 ;

	delete psn_info_packet_ ;
	psn_info_packet_ = 0 ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void
psn_encoder::
set_trackers( const psn_tracker_array & trackers )
{
	trackers_ = trackers ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_encoder::
encode_info( ::std::list< ::std::string > & encoded_info_packets , uint64_t timestamp /*= 0*/ ) 
{
	// Update PSN_INFO_TRACKER_LIST chunk and split all the PSN_INFO_TRACKER chunks into multiple packets
	::std::vector< ::std::vector< psn_chunk * > > psn_info_tracker_packets ;

	{
		psn_info_tracker_list_->remove_all_children() ;

		size_t static_chunks_size = psn_info_packet_->get_chunk_size() ;
		size_t buffer_size = static_chunks_size ;

		::std::vector< psn_chunk * > psn_info_tracker_packet ;

		::psn::psn_tracker_array::iterator it ;
		for ( it = trackers_.begin() ; it != trackers_.end() ; ++it )
		{
			psn_chunk * psn_info_tracker = new psn_chunk( it->second.id_ , 0 , "PSN_INFO_TRACKER" ) ;

			psn_info_tracker->add_child( new psn_chunk( PSN_INFO_TRACKER_NAME , new psn_name_data( it->second.name_ ) , "PSN_INFO_TRACKER_NAME" ) ) ;
			
			if ( !psn_info_tracker_packet.empty() && // be sure to push at least one tracker per packet
				 buffer_size + psn_info_tracker->get_chunk_size() > PSN_MAX_UDP_PACKET_SIZE )
			{
				psn_info_tracker_packets.push_back( psn_info_tracker_packet ) ;
				psn_info_tracker_packet.clear() ;
				
				buffer_size = static_chunks_size ;
			}
			
			buffer_size += psn_info_tracker->get_chunk_size() ;
			psn_info_tracker_packet.push_back( psn_info_tracker ) ;
		}		

		if ( !psn_info_tracker_packet.empty() )
			psn_info_tracker_packets.push_back( psn_info_tracker_packet ) ;
	}

	// Update PSN_INFO_PACKET_HEADER chunk
	{
		psn_packet_header_data * data = (psn_packet_header_data *)psn_info_packet_header_->get_data() ;

		if ( data != 0 )
		{
			data->timestamp_usec_ = timestamp ;
			data->version_high_ = PSN_HIGH_VERSION ;
			data->version_low_ = PSN_LOW_VERSION ;
			data->frame_id_ = ++info_packet_frame_id_ ;
			data->frame_packet_count_ = (uint8_t)psn_info_tracker_packets.size() ;
		}
	}

	// Encode all packets
	for ( size_t i = 0 ; i < psn_info_tracker_packets.size() ; ++i )
	{
		psn_info_tracker_list_->remove_all_children() ;

		::std::vector< psn_chunk * > & psn_info_tracker_packet = psn_info_tracker_packets[ i ] ;

		for ( size_t j = 0 ; j < psn_info_tracker_packet.size() ; ++j )
		{
			psn_info_tracker_list_->add_child( psn_info_tracker_packet[ j ] ) ;
		}

		size_t buffer_size = psn_info_packet_->get_chunk_size() ;

		char * buffer = (char *)malloc( buffer_size ) ;

		if ( psn_info_packet_->encode( buffer , buffer_size ) )
		{
			encoded_info_packets.push_back( ::std::string( buffer , buffer_size ) ) ;
		}

		free( buffer ) ;
	}

	return true ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_encoder::
encode_data( ::std::list< ::std::string > & encoded_data_packets , uint64_t timestamp /*= 0*/ ) 
{
	// Update PSN_TRACKER_SECTION chunk and split all the PSN_DATA_TRACKER chunks into multiple packets
	::std::vector< ::std::vector< psn_chunk * > > psn_data_tracker_packets ;

	{
		psn_data_tracker_list_->remove_all_children() ;

		size_t static_chunks_size = psn_data_packet_->get_chunk_size() ;
		size_t buffer_size = static_chunks_size ;

		::std::vector< psn_chunk * > psn_data_tracker_packet ;

		::psn::psn_tracker_array::iterator it ;
		for ( it = trackers_.begin() ; it != trackers_.end() ; ++it )
		{
			psn_chunk * psn_data_tracker = new psn_chunk( it->second.id_ , 0 , "PSN_DATA_TRACKER" ) ;

			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_POS	, new psn_vec3_data( it->second.pos_.x   , it->second.pos_.y   , it->second.pos_.z )	, "PSN_DATA_TRACKER_POS" ) ) ;
			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_SPEED	, new psn_vec3_data( it->second.speed_.x , it->second.speed_.y , it->second.speed_.z )	, "PSN_DATA_TRACKER_SPEED" ) ) ;
			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_ORI	, new psn_vec3_data( it->second.ori_.x   , it->second.ori_.y   , it->second.ori_.z )	, "PSN_DATA_TRACKER_ORI" ) ) ;
			psn_data_tracker->add_child( new psn_chunk( PSN_DATA_TRACKER_STATUS , new psn_tracker_status_data( it->second.validity_ )									, "PSN_DATA_TRACKER_STATUS" ) ) ;

			if ( !psn_data_tracker_packet.empty() && // be sure to push at least one tracker per packet
				 buffer_size + psn_data_tracker->get_chunk_size() > PSN_MAX_UDP_PACKET_SIZE )
			{
				psn_data_tracker_packets.push_back( psn_data_tracker_packet ) ;
				psn_data_tracker_packet.clear() ;
				
				buffer_size = static_chunks_size ;
			}

			buffer_size += psn_data_tracker->get_chunk_size() ;
			psn_data_tracker_packet.push_back( psn_data_tracker ) ;
		}

		if ( !psn_data_tracker_packet.empty() )
			psn_data_tracker_packets.push_back( psn_data_tracker_packet ) ;
	}

	// Update PSN_DATA_PACKET_HEADER chunk
	{
		psn_packet_header_data * data = (psn_packet_header_data *)psn_data_packet_header_->get_data() ;

		if ( data != 0 )
		{
			data->timestamp_usec_ = timestamp ;
			data->version_high_ = PSN_HIGH_VERSION ;
			data->version_low_ = PSN_LOW_VERSION ;
			data->frame_id_ = ++data_packet_frame_id_ ;
			data->frame_packet_count_ = (uint8_t)psn_data_tracker_packets.size() ;
		}
	}

	// Encode all packets
	for ( size_t i = 0 ; i < psn_data_tracker_packets.size() ; ++i )
	{
		psn_data_tracker_list_->remove_all_children() ;

		::std::vector< psn_chunk * > & psn_data_tracker_packet = psn_data_tracker_packets[ i ] ;

		for ( int j = 0 ; j < (int)psn_data_tracker_packet.size() ; ++j )
		{
			psn_data_tracker_list_->add_child( psn_data_tracker_packet[ j ] ) ;
		}

		size_t buffer_size = psn_data_packet_->get_chunk_size() ;

		char * buffer = (char *)malloc( buffer_size ) ;

		if ( psn_data_packet_->encode( buffer , buffer_size ) )
		{
			encoded_data_packets.push_back( ::std::string( buffer , buffer_size ) ) ;
		}

		free( buffer ) ;
	}

	return true ;
}


} // namespace psn

#endif
