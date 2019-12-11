#ifndef __DBCAPI_INCLUDED__
#define __DBCAPI_INCLUDED__

typedef enum a_ads_data_type
{
    /// Invalid data type.
    A_INVALID_TYPE,
    /// Binary data.  Binary data is treated as-is and no character set conversion is performed.
    A_BINARY,
    /// String data.  The data where character set conversion is performed.
    A_STRING,
    /// Double data.  Includes float values.
    A_DOUBLE,
    /// 64-bit integer.
    A_VAL64,
    /// 64-bit unsigned integer.
    A_UVAL64,
    /// 32-bit integer.
    A_VAL32,
    /// 32-bit unsigned integer.
    A_UVAL32,
    /// 16-bit integer.
    A_VAL16,
    /// 16-bit unsigned integer.
    A_UVAL16,
    /// 8-bit integer.
    A_VAL8,
    /// 8-bit unsigned integer.
    A_UVAL8,
    /// Unicode data (NCHAR)
    A_NCHAR,
    // Numeric data (lossless)
    A_DECIMAL,
    // Date value
    A_DATE,
    // Time value
    A_TIME,
    // Timestamp
    A_TIMESTAMP

} a_ads_data_type;

typedef struct a_ads_data_value
{
    /// A pointer to user supplied buffer of data.
    UNSIGNED8 	   *buffer;
    /// The size of the buffer.
    UNSIGNED32    buffer_size;
    /// A pointer to the number of valid bytes in the buffer.  This value must be less than buffer_size.
    UNSIGNED32    *length;
    /// The type of the data
    a_ads_data_type	type;
    /// A pointer to indicate whether the last fetched data is NULL.
    UNSIGNED32    *is_null;
} a_ads_data_value;

/** A data direction enumeration.
 */
typedef enum a_ads_data_direction
{
	/// Invalid data direction.
    DD_INVALID       = 0x0,
	/// Input-only host variables.
    DD_INPUT         = 0x1,
	/// Output-only host variables.
    DD_OUTPUT        = 0x2,
	/// Input and output host variables.
    DD_INPUT_OUTPUT  = 0x3
} a_ads_data_direction;

typedef struct a_ads_bind_param
{
    /// The direction of the data. (input, output, input_output)
    a_ads_data_direction	direction;
    /// The actual value of the data.
    a_ads_data_value		value;
    /// Name of the bind parameter. This is only used by ads_describe_bind_param().
    UNSIGNED8        *name;
} a_ads_bind_param;

typedef enum a_ads_native_type
{
    /// No data type.
    DT_NOTYPE       	= 0,
    /// Null-terminated character string that is a valid date. 
    DT_DATE         	= 384,
    /// Null-terminated character string that is a valid time. 
    DT_TIME         	= 388,
    /// Null-terminated character string that is a valid timestamp.
    DT_TIMESTAMP    	= 392,
    /// Varying length character string, in the CHAR character set, with a two-byte length field. The maximum length is 32765 bytes . When sending data, you must set the length field. When fetching data, the database server sets the length field. The data is not null-terminated or blank-padded.
    DT_VARCHAR      	= 448,
    /// Fixed-length blank-padded character string, in the CHAR character set. The maximum length, specified in bytes, is 32767. The data is not null-terminated. 
    DT_FIXCHAR      	= 452,
    /// Long varying length character string, in the CHAR character set.
    DT_LONGVARCHAR  	= 456,
    /// Null-terminated character string, in the CHAR character set. The string is blank-padded if the database is initialized with blank-padded strings.
    DT_STRING       	= 460,
    /// 8-byte floating-point number. 
    DT_DOUBLE       	= 480,
    /// 4-byte floating-point number.
    DT_FLOAT        	= 482,
    /// Packed decimal number (proprietary format). 
    DT_DECIMAL      	= 484,
    /// 32-bit signed integer.
    DT_INT          	= 496,
    /// 16-bit signed integer.
    DT_SMALLINT     	= 500,
    /// Varying length binary data with a two-byte length field. The maximum length is 32765 bytes. When supplying information to the database server, you must set the length field. When fetching information from the database server, the server sets the length field. 
    DT_BINARY       	= 524,
    /// Long binary data.
    DT_LONGBINARY   	= 528,
    /// 8-bit signed integer.
    DT_TINYINT      	= 604,
    /// 64-bit signed integer.
    DT_BIGINT       	= 608,
    /// 32-bit unsigned integer.
    DT_UNSINT       	= 612,
    /// 16-bit unsigned integer.
    DT_UNSSMALLINT  	= 616,
    /// 64-bit unsigned integer.
    DT_UNSBIGINT    	= 620,
    /// 8-bit signed integer.
    DT_BIT          	= 624,
    // Null-terminated string, in NCHAR character set. The string is blank-padded if the database is initialized with blank-padded strings.This variable holds n-1 characters plus the null terminator.
    DT_NSTRING       = 628,
    // Fixed length character string, in NCHAR character set. Blank-padded but not null-terminated. The maximum value for n is 32767.
    DT_NFIXCHAR      = 632,
    // Varying length character string, in NCHAR character set, with 2-byte length field. Not null-terminated or blank-padded. The maximum value for n is 32765.
    DT_NVARCHAR      = 636,
    /// Long varying length character string, in the NCHAR character set.
    DT_LONGNVARCHAR 	= 640

} a_ads_native_type;

typedef struct a_ads_column_info
{
    /// The name of the column (null-terminated).
    /// The string can be referenced as long as the result set object is not freed.
    UNSIGNED8        *name;
    /// The column data type.
    a_ads_data_type		type;
    /// The native type of the column in the database.
    a_ads_native_type	native_type;
    /// The precision.
    UNSIGNED16          precision;
    /// The scale.
    UNSIGNED16          scale;
    /// The maximum size a data value in this column can take.
    UNSIGNED32          max_size;
    /// Indicates whether a value in the column can be null.
    UNSIGNED32          nullable;
} a_ads_column_info;

typedef struct a_ads_bind_param_info
{
    /// A pointer to the name of the parameter.
    UNSIGNED8                 *name;
    /// The direction of the parameter.
    a_ads_data_direction 	direction;
    /// Information about the bound input value.
    a_ads_data_value		input_value;
    /// Information about the bound output value.
    a_ads_data_value		output_value;
} a_ads_bind_param_info;

typedef struct a_ads_data_info
{
    /// The type of the data in the column.
    a_ads_data_type	type;
    /// Indicates whether the last fetched data is NULL.
    /// This field is only valid after a successful fetch operation.
    UNSIGNED32          is_null;
    /// The total number of bytes available to be fetched.
    /// This field is only valid after a successful fetch operation.
    UNSIGNED32          data_size;
} a_ads_data_info;


// Dummy connection object we can create & destroy.  This is needed since DBCAPI wants
// a connection object from the new_connection API.  If we need to store connection
// specific info for DBCAPI connections we could store it in this structure as opposed
// to the ACE CCONNECTION object.
typedef struct A_ADS_CONNECTION_STRUCT
{
   ADSHANDLE hConnect;  // Real ADS connection, set via ads_connect

} a_ads_connection, *a_ads_connection_ptr;

typedef struct 
{
   UNSIGNED8  *pucData;       // Data buffer for the column
   UNSIGNED8  *pucColumnName; // Column name string
   UNSIGNED32 ulBufferLength; // Length of buffer
   UNSIGNED32 ulDataLength;   // Length of data in the buffer
   UNSIGNED32 ulIsNull;       // T/F NULL flag

} DBCAPI_COLUMN;


// DBCAPI entrypoints (not published)
#ifdef __cplusplus
   extern "C"
   {
#endif

UNSIGNED32 ENTRYPOINT ads_affected_rows( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_bind_param( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param *param );
void ENTRYPOINT ads_cancel( a_ads_connection *poConnect );
void ENTRYPOINT ads_clear_error( a_ads_connection *poConnect );
UNSIGNED32 ENTRYPOINT ads_client_version( UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength );
UNSIGNED32 ENTRYPOINT ads_commit( a_ads_connection *poConnect );
UNSIGNED32 ENTRYPOINT ads_connect( a_ads_connection *poConnect, UNSIGNED8 *pucConnectString );
UNSIGNED32 ENTRYPOINT ads_describe_bind_param( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param *param );
UNSIGNED32 ENTRYPOINT ads_disconnect( a_ads_connection *poConnect );
UNSIGNED32 ENTRYPOINT ads_error( a_ads_connection *poConnect, UNSIGNED8 *pucError, UNSIGNED32 ulLength );
UNSIGNED32 ENTRYPOINT ads_execute( ADSHANDLE hStatement );
ADSHANDLE ENTRYPOINT ads_execute_direct( a_ads_connection *poConnect, UNSIGNED8 *pucSQL );
UNSIGNED32 ENTRYPOINT ads_execute_immediate( a_ads_connection *poConnect, UNSIGNED8 *pucSQL );
UNSIGNED32 ENTRYPOINT ads_fetch_absolute( ADSHANDLE hStatement, UNSIGNED32 ulRowNum );
UNSIGNED32 ENTRYPOINT ads_fetch_next( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_fini( void );
UNSIGNED32 ENTRYPOINT ads_free_connection( a_ads_connection *poConnect );
UNSIGNED32 ENTRYPOINT ads_free_stmt( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_get_bind_param_info( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param_info *info );
UNSIGNED32 ENTRYPOINT ads_get_column( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_data_value *buffer );
UNSIGNED32 ENTRYPOINT ads_get_column_info( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_column_info *buffer );
UNSIGNED32 ENTRYPOINT ads_get_data( ADSHANDLE hStatement, UNSIGNED32 ulIndex, UNSIGNED32 ulOffset, void *buffer, UNSIGNED32 ulLength );
UNSIGNED32 ENTRYPOINT ads_get_data_info( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_data_info *buffer );
UNSIGNED32 ENTRYPOINT ads_get_next_result( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_init( UNSIGNED8 *app_name, UNSIGNED32 api_version, UNSIGNED32 *version_available );
a_ads_connection_ptr ENTRYPOINT ads_make_connection( void *arg );
a_ads_connection_ptr ENTRYPOINT ads_new_connection( void );
UNSIGNED32 ENTRYPOINT ads_num_cols( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_num_params( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_num_rows( ADSHANDLE hStatement );
ADSHANDLE ENTRYPOINT ads_prepare( a_ads_connection *poConnect, UNSIGNED8 *pucSQL, UNSIGNED8 ucIsUnicode );
UNSIGNED32 ENTRYPOINT ads_reset( ADSHANDLE hStatement );
UNSIGNED32 ENTRYPOINT ads_rollback( a_ads_connection *poConnect );
UNSIGNED32 ENTRYPOINT ads_send_param_data( ADSHANDLE hStatement, UNSIGNED32 ulIndex, UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength );
UNSIGNED32 ENTRYPOINT ads_sqlstate( a_ads_connection *poConnect, UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength );


#ifdef __cplusplus
   }
#endif

#endif // __DBCAPI_INCLUDED__

