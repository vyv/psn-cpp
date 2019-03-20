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

#ifndef PSN_GENERIC_HPP
#define PSN_GENERIC_HPP

#include <psn_defs.hpp>

namespace psn
{

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_chunk_header
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
struct psn_chunk_header
{
	union
    {
        struct
        {
            uint32_t    id_            : 16 ;   // chunk ID
            uint32_t    data_len_      : 15 ;   // length of chunk data or sub chunks
            uint32_t    has_subchunks_ : 1 ;    // does it contain other chunks
        } ;

        uint32_t bits_ ;						// to set all bits at once
    } ;
} ;

static size_t SIZE_OF_PSN_CHUNK_HEADER = sizeof( psn_chunk_header ) ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_chunk
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

// Forward declaration of psn_chunk_data
class psn_chunk_data ;

class psn_chunk
{
  public :

	psn_chunk( uint16_t id , psn_chunk_data * data = 0 , const ::std::string & name = "" ) ;
	virtual ~psn_chunk( void ) ;

	uint16_t get_id( void ) const ;

	void set_name( const ::std::string & name ) ;
	::std::string get_name( void ) const ;

	size_t get_chunk_size( void ) const ;
	psn_chunk_data * get_data( void ) const ;

	bool encode( void * buffer , size_t buffer_size ) ;
	bool decode( void * buffer , size_t buffer_size ) ;

  protected :
	
	virtual bool decode_child( uint16_t child_id , void * buffer , size_t buffer_size ) ;

  public :

	psn_chunk * get_parent( void ) const ;

	void add_child( psn_chunk * child ) ;
	void insert_child( int index , psn_chunk * child ) ;

	bool remove_child_by_index( int index , bool auto_delete = true ) ;
	bool remove_child_by_id( uint16_t id , bool auto_delete = true ) ;
	bool remove_child( psn_chunk * child , bool auto_delete = true ) ;

	void remove_all_children( bool auto_delete = true ) ;

	psn_chunk * get_child_by_id( uint16_t id ) ;
	::std::vector< psn_chunk * > get_children( void ) ;

	bool find_child( psn_chunk * child ) ;
	bool find_parent( psn_chunk * parent ) ;

  private :	

	psn_chunk *						parent_ ;
	::std::vector< psn_chunk * >	children_ ;

  private :
	uint16_t						id_ ;
	::std::string					name_ ;	

	psn_chunk_header				header_ ;
	psn_chunk_data					* data_ ;
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_chunk_data
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_chunk_data
{
  public :

	psn_chunk_data( void ) {}
	virtual ~psn_chunk_data( void ) {}

	virtual size_t get_data_size( void ) const = 0 ;
	virtual void write_buffer( void * buffer ) const = 0 ;
	virtual void read_buffer( void * buffer , size_t buffer_size ) = 0 ;
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_packet_header_data
//
// It is used by :
// ----------------------
// PSN_INFO_PACKET_HEADER 
// PSN_DATA_PACKET_HEADER
//-----------------------
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_packet_header_data : public psn_chunk_data
{
  public :

	psn_packet_header_data( void )
		: timestamp_usec_( 0 )
		, version_high_( 0 )
		, version_low_( 0 )
		, frame_id_( 0 )
		, frame_packet_count_( 0 )
	{}
	virtual ~psn_packet_header_data( void ) {}

	virtual size_t get_data_size( void ) const
	{
		static size_t size_of_data = sizeof( timestamp_usec_ ) +
									 sizeof( version_high_ ) +
									 sizeof( version_low_ ) +
									 sizeof( frame_id_ ) +
									 sizeof( frame_packet_count_ ) ;

		return size_of_data ;
	}

	virtual void write_buffer( void * buffer ) const
	{
		memcpy( buffer , &timestamp_usec_ , sizeof( timestamp_usec_ ) ) ;
		
		buffer = (char *)buffer + sizeof( timestamp_usec_ ) ;
		memcpy( buffer , &version_high_ , sizeof( version_high_ ) ) ;
		
		buffer = (char *)buffer + sizeof( version_high_ ) ;
		memcpy( buffer , &version_low_ , sizeof( version_low_ ) ) ;
		
		buffer = (char *)buffer + sizeof( version_low_ ) ;
		memcpy( buffer , &frame_id_ , sizeof( frame_id_ ) ) ;
		
		buffer = (char *)buffer + sizeof( frame_id_ ) ;
		memcpy( buffer , &frame_packet_count_ , sizeof( frame_packet_count_ ) ) ;
	}

	virtual void read_buffer( void * buffer , size_t buffer_size ) 
	{
		if ( buffer_size < get_data_size() )
			return ;

		memcpy( &timestamp_usec_ , buffer , sizeof( timestamp_usec_ ) ) ;
		
		buffer = (char *)buffer + sizeof( timestamp_usec_ ) ;
		memcpy( &version_high_ , buffer , sizeof( version_high_ ) ) ;
		
		buffer = (char *)buffer + sizeof( version_high_ ) ;
		memcpy( &version_low_ , buffer , sizeof( version_low_ ) ) ;
		
		buffer = (char *)buffer + sizeof( version_low_ ) ;
		memcpy( &frame_id_ , buffer , sizeof( frame_id_ ) ) ;
		
		buffer = (char *)buffer + sizeof( frame_id_ ) ;
		memcpy( &frame_packet_count_ , buffer , sizeof( frame_packet_count_ ) ) ;
	}

  public :

	uint64_t timestamp_usec_ ;
    uint8_t  version_high_ ;
    uint8_t  version_low_ ;
    uint8_t  frame_id_ ;
    uint8_t  frame_packet_count_ ;  
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_vec3_data
//
// It is used by :
// ----------------------
// PSN_DATA_TRACKER_POS 
// PSN_DATA_TRACKER_SPEED
// PSN_DATA_TRACKER_ORI
//-----------------------
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_vec3_data : public psn_chunk_data
{
  public :

	psn_vec3_data( float x = 0 , float y = 0 , float z = 0 )
		: vec_( x , y , z )
	{}

	virtual ~psn_vec3_data( void ) {}

	virtual size_t get_data_size( void ) const
	{
		static size_t size_of_data = sizeof( vec_ ) ;

		return size_of_data ;
	}

	virtual void write_buffer( void * buffer ) const
	{
		memcpy( buffer , &vec_ , sizeof( vec_ ) ) ;
	}

	virtual void read_buffer( void * buffer , size_t buffer_size ) 
	{
		if ( buffer_size < get_data_size() )
			return ;

		memcpy( &vec_ , buffer , sizeof( vec_ ) ) ;
	}

  public :	

	float3	vec_ ;  
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_vec3_data
//
// It is used by :
// ----------------------
// PSN_DATA_TRACKER_STATUS
//-----------------------
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_tracker_status_data : public psn_chunk_data
{
  public :

	psn_tracker_status_data( float validity = 0.f ) 
		: validity_( validity )
	{}

	virtual ~psn_tracker_status_data( void ) {}

	virtual size_t get_data_size( void ) const
	{
		static size_t size_of_data = sizeof( validity_ ) ;

		return size_of_data ;
	}

	virtual void write_buffer( void * buffer ) const
	{
		memcpy( buffer , &validity_ , sizeof( validity_ ) ) ;
	}

	virtual void read_buffer( void * buffer , size_t buffer_size ) 
	{
		if ( buffer_size < get_data_size() )
			return ;

		memcpy( &validity_ , buffer , sizeof( validity_ ) ) ;
	}

  public :	

	float	validity_ ;  
} ;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// psn_name_data
//
// It is used by :
// ----------------------
// PSN_INFO_SYSTEM_NAME
// PSN_INFO_TRACKER_NAME
//-----------------------
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class psn_name_data : public psn_chunk_data
{
  public :

	psn_name_data( const ::std::string & name = "" ) 
		: name_( name )
	{}

	virtual ~psn_name_data( void ) {}

	virtual size_t get_data_size( void ) const
	{
		return name_.length() ;
	}

	virtual void write_buffer( void * buffer ) const
	{
		memcpy( buffer , name_.c_str() , name_.length() ) ;
	}

	virtual void read_buffer( void * buffer , size_t buffer_size ) 
	{
		name_.clear() ;

		if ( buffer_size > 0 )
			name_ = ::std::string( (char *)buffer , buffer_size ) ;
	}

  public :	

	::std::string	name_ ;  
} ;

} // namespace psn

#endif
