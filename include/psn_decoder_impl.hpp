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

#ifndef PSN_DECODER_IMPL_HPP
#define PSN_DECODER_IMPL_HPP

#include <psn_decoder.hpp> 

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace psn
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_decoder::
psn_decoder( void )
	: last_frame_id_( 0 )
	, frame_packet_counter_( 0 )
{
	// PSN_DATA 
	psn_data_packet_			= new psn_chunk					 ( PSN_DATA_PACKET			, 0										, "PSN_DATA_PACKET" ) ;
	psn_data_packet_header_		= new psn_chunk					 ( PSN_DATA_PACKET_HEADER	, new ::psn::psn_packet_header_data()	, "PSN_DATA_PACKET_HEADER" ) ;
	psn_data_tracker_list_		= new psn_data_tracker_list_chunk( PSN_DATA_TRACKER_LIST	, 0										, "PSN_DATA_TRACKER_LIST" ) ;
	// PSN_DATA_TRACKER chunks will be dynamically instantiated on decode_data 

	psn_data_packet_->add_child( psn_data_packet_header_ ) ;
	psn_data_packet_->add_child( psn_data_tracker_list_ ) ;

	// PSN_INFO
	psn_info_packet_			= new psn_chunk					 ( PSN_INFO_PACKET			, 0										, "PSN_INFO_PACKET" ) ;
	psn_info_packet_header_		= new psn_chunk					 ( PSN_INFO_PACKET_HEADER	, new ::psn::psn_packet_header_data()	, "PSN_INFO_PACKET_HEADER" ) ;
	psn_info_system_name_		= new psn_chunk					 ( PSN_INFO_SYSTEM_NAME		, new ::psn::psn_name_data()			, "PSN_INFO_SYSTEM_NAME" ) ;
	psn_info_tracker_list_		= new psn_info_tracker_list_chunk( PSN_INFO_TRACKER_LIST	, 0										, "PSN_INFO_TRACKER_LIST" ) ;
	// PSN_INFO_TRACKER chunks will be dynamically instantiated on decode_info

	psn_info_packet_->add_child( psn_info_packet_header_ ) ;
	psn_info_packet_->add_child( psn_info_system_name_ ) ;
	psn_info_packet_->add_child( psn_info_tracker_list_ ) ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
psn_decoder::
~psn_decoder( void ) 
{
	delete psn_data_packet_ ;
	psn_data_packet_ = 0 ;

	delete psn_info_packet_ ;
	psn_info_packet_ = 0 ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
uint8_t 
psn_decoder::
get_last_decoded_frame_id( void ) const 
{
	return last_frame_id_ ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
const psn_tracker_array &
psn_decoder::
get_trackers( void ) const
{
	return trackers_ ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_decoder::
get_psn_server_name( ::std::string & server_name )
{
	if ( psn_info_system_name_ != 0 )
	{
		::psn::psn_name_data * data = (::psn::psn_name_data *)psn_info_system_name_->get_data() ;

		if ( data != 0 )
		{
			server_name = data->name_ ;
			return true ;
		}
	}

	return false ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_decoder::
decode( const ::std::string & packet )
{
	const char * buffer = packet.c_str() ;
	size_t buffer_size = packet.length() ;

	if ( buffer_size < SIZE_OF_PSN_CHUNK_HEADER ) 
		return false ;

	psn_chunk_header chunk_header ;

	// decode root chunk header
	memcpy( &chunk_header , buffer , SIZE_OF_PSN_CHUNK_HEADER ) ;

	if ( chunk_header.id_ == psn_data_packet_->get_id() )
	{
		return decode_data_packet( (void *)buffer , buffer_size ) ;
	}
	else if ( chunk_header.id_ == psn_info_packet_->get_id() )
	{
		return decode_info_packet( (void *)buffer , buffer_size ) ;
	}

	return false ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_decoder::
decode_data_packet( void * buffer , size_t buffer_size ) 
{
	// Remove all children from PSN_DATA_TRACKER_LIST chunk
	// It will add its own children during the decode process
	psn_data_tracker_list_->remove_all_children() ;

	if ( !psn_data_packet_->decode( buffer , buffer_size ) )
		return false ;

	bool last_packet = false ;

	// Check if it's the last packet for this frame
	{
		psn_packet_header_data * data = (psn_packet_header_data *)psn_data_packet_header_->get_data() ;

		if ( data != 0 )
		{
			// Backup solution in case data->frame_packet_count_ is bad or we missed a packet.
            bool expect_new_frame = ( frame_packet_counter_ == 0 ) ;

			if ( data->frame_id_ != last_frame_id_ && !expect_new_frame )
			{
				frame_packet_counter_ = 0 ;

				trackers_ = frame_trackers_ ;
				frame_trackers_.clear() ;
			}

            last_frame_id_ = data->frame_id_ ;

			// Is it the last packet ?
			if ( data->frame_packet_count_ == ++frame_packet_counter_ )
			{
				last_packet = true ; // Will copy frame_trackers_ to trackers_ at the end.
				frame_packet_counter_ = 0 ;
			}
		}
	}

	// Get all the PSN_DATA_TRACKER chunks and extract the trackers data from them 
	::std::vector< ::psn::psn_chunk * > trackers_chunk =
		psn_data_tracker_list_->get_children() ;

	for ( size_t i = 0 ; i < trackers_chunk.size() ; ++i )
	{
		psn_tracker t ;
		t.id_ = trackers_chunk[ i ]->get_id() ;

		// Copy tracker info
		if ( trackers_info_.find( t.id_ ) != trackers_info_.end() )
			t.name_ = trackers_info_[ t.id_ ] ;

		// Tracker Pos
		{
			::psn::psn_chunk * tracker_pos_chunk = trackers_chunk[ i ]->get_child_by_id( PSN_DATA_TRACKER_POS ) ;
			if ( tracker_pos_chunk != 0 )
			{
				psn_vec3_data * pos_data = (psn_vec3_data *)tracker_pos_chunk->get_data() ;

				if ( pos_data != 0 )
				{
					t.pos_.x = pos_data->vec_.x ;
					t.pos_.y = pos_data->vec_.y ;
					t.pos_.z = pos_data->vec_.z ;
				}
			}
		}

		// Tracker Speed
		{
			::psn::psn_chunk * tracker_speed_chunk = trackers_chunk[ i ]->get_child_by_id( PSN_DATA_TRACKER_SPEED ) ;
			if ( tracker_speed_chunk != 0 )
			{
				psn_vec3_data * speed_data = (psn_vec3_data *)tracker_speed_chunk->get_data() ;

				if ( speed_data != 0 )
				{
					t.speed_.x = speed_data->vec_.x ;
					t.speed_.y = speed_data->vec_.y ;
					t.speed_.z = speed_data->vec_.z ;
				}
			}
		}

		// Tracker Ori
		{
			::psn::psn_chunk * tracker_ori_chunk = trackers_chunk[ i ]->get_child_by_id( PSN_DATA_TRACKER_ORI ) ;
			if ( tracker_ori_chunk != 0 )
			{
				psn_vec3_data * ori_data = (psn_vec3_data *)tracker_ori_chunk->get_data() ;

				if ( ori_data != 0 )
				{
					t.ori_.x = ori_data->vec_.x ;
					t.ori_.y = ori_data->vec_.y ;
					t.ori_.z = ori_data->vec_.z ;
				}
			}
		}

		// Tracker status
		{
			::psn::psn_chunk * tracker_status_chunk = trackers_chunk[ i ]->get_child_by_id( PSN_DATA_TRACKER_STATUS ) ;
			if ( tracker_status_chunk != 0 )
			{
				psn_tracker_status_data * status_data = (psn_tracker_status_data *)tracker_status_chunk->get_data() ;

				if ( status_data != 0 )
				{
					t.validity_ = status_data->validity_ ;
				}
			}
		}

		frame_trackers_[ t.id_ ] = t ;
	}

	if ( last_packet )
	{
		trackers_ = frame_trackers_ ;
		frame_trackers_.clear() ;
	}

	return true ;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
bool
psn_decoder::
decode_info_packet( void * buffer , size_t buffer_size ) 
{
	// Remove all children from PSN_INFO_TRACKER_LIST chunk
	// It will add its own children during the decode process
	psn_info_tracker_list_->remove_all_children() ;

	if ( !psn_info_packet_->decode( (void *)buffer , buffer_size ) )
		return false ;

	// Get all the PSN_INFO_TRACKER chunks and extract the trackers name from them 
	::std::vector< ::psn::psn_chunk * > trackers_chunk =
		psn_info_tracker_list_->get_children() ;

	for ( size_t i = 0 ; i < trackers_chunk.size() ; ++i )
	{
		uint16_t tracker_id = trackers_chunk[ i ]->get_id() ;

		// Tracker Name
		{
			::psn::psn_chunk * tracker_name_chunk = trackers_chunk[ i ]->get_child_by_id( PSN_INFO_TRACKER_NAME ) ;
			if ( tracker_name_chunk != 0 )
			{
				psn_name_data * name_data = (psn_name_data *)tracker_name_chunk->get_data() ;

				if ( name_data != 0 )
				{
					trackers_info_[ tracker_id ] = name_data->name_ ;
				}
			}
		}
	}

	return true ;
}

} // namespace psn

#endif
