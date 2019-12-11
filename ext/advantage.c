/*====================================================
*
*   Copyright 2008-2010 iAnywhere Solutions, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the License for the specific language governing permissions and
* limitations under the License.
*
*
*====================================================*/
#include "ruby.h"
#include "adscapidll.h"

const char* VERSION = "0.1.1";

typedef struct imp_drh_st
   {
   AdvantageInterface  api;
   void                *adscapi_context;
   } imp_drh_st;

// Defining the Ruby Modules
static VALUE mAdvantage;
static VALUE mAPI;
static VALUE cAdvantageInterface;

// Defining binder for DBCAPI types
static VALUE cA_ads_connection;
static VALUE cA_ads_data_value;
static VALUE cA_ads_bind_param;
static VALUE cA_ads_bind_param_info;

// This function is called when the module is first loaded by ruby.
// The name of this function MUST match be Init_<modulename>.
void Init_advantage(  );

// Wrapper functions for the DBICAPI functions

static VALUE
static_API_ads_initialize_interface( VALUE module, VALUE imp_drh );

static VALUE
static_API_ads_finalize_interface( VALUE module, VALUE imp_drh );

static VALUE
static_AdvantageInterface_alloc( VALUE class );

static VALUE
static_AdvantageInterface_ads_init( VALUE class );

static VALUE
static_AdvantageInterface_ads_new_connection( VALUE class );

static VALUE
static_AdvantageInterface_ads_client_version( VALUE imp_drh );

static VALUE
static_AdvantageInterface_ads_connect( VALUE imp_drh, VALUE ads_conn, VALUE str );

static VALUE
static_AdvantageInterface_ads_disconnect( VALUE imp_drh, VALUE ads_conn );

static VALUE
static_AdvantageInterface_ads_free_connection( VALUE imp_drh, VALUE ads_conn );

static VALUE
static_AdvantageInterface_ads_fini( VALUE imp_drh );

static VALUE
static_AdvantageInterface_ads_error( VALUE imp_drh, VALUE ads_conn );

static VALUE
static_AdvantageInterface_ads_execute_immediate( VALUE imp_drh, VALUE ads_conn, VALUE sql );

static VALUE
static_AdvantageInterface_ads_execute_direct( VALUE imp_drh, VALUE ads_conn, VALUE sql );

static VALUE
static_AdvantageInterface_ads_num_cols( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_num_rows( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_get_column( VALUE imp_drh, VALUE ads_stmt, VALUE col_num );

static VALUE
static_AdvantageInterface_ads_fetch_next( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_get_column_info( VALUE imp_drh, VALUE ads_stmt, VALUE col_num );

static VALUE
static_AdvantageInterface_AdsBeginTransaction( VALUE imp_drh, VALUE ads_conn );

static VALUE
static_AdvantageInterface_ads_commit( VALUE imp_drh, VALUE ads_conn );

static VALUE
static_AdvantageInterface_ads_rollback( VALUE imp_drh, VALUE ads_conn );

static VALUE
static_AdvantageInterface_ads_prepare( VALUE imp_drh, VALUE ads_conn, VALUE sql );

static VALUE
static_AdvantageInterface_ads_free_stmt( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_reset( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_execute( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_affected_rows( VALUE imp_drh, VALUE ads_stmt );

static VALUE
static_AdvantageInterface_ads_describe_bind_param( VALUE imp_drh, VALUE ads_stmt, VALUE index );

/*
 * C to Ruby Data conversion function to convert DBCAPI column type into the correct Ruby type
 */
static VALUE C2RB( a_ads_data_value* value )
   {
   VALUE tdata;

   if( value == NULL || value->buffer == NULL || value->is_null == NULL )
      {
      rb_raise( rb_eTypeError, "Operation not allowed. Has no value." );
      }
   else if( *value->is_null )
      {
      tdata = Qnil;
      }
   else
      {
      switch( value->type )
         {
         case A_BINARY:
            tdata = rb_str_new( ( char* )value->buffer, *value->length );
            break;
         case A_STRING:
         case A_DATE:
         case A_TIME:
         case A_TIMESTAMP:
         case A_NCHAR:
         case A_DECIMAL:
            tdata = rb_str_new( ( char* )value->buffer, *value->length );
            break;
         case A_DOUBLE:
            tdata = rb_float_new( *( double* ) value->buffer );
            break;
         case A_VAL64:
            tdata = LL2NUM( *( LONG_LONG* )value->buffer );
            break;
         case A_UVAL64:
            tdata = ULL2NUM( *( unsigned LONG_LONG* )value->buffer );
            break;
         case A_VAL32:
            tdata = INT2NUM( *( int * )value->buffer );
            break;
         case A_UVAL32:
            tdata = UINT2NUM( *( unsigned int * )value->buffer );
            break;
         case A_VAL16:
            tdata = INT2NUM( *( short * )value->buffer );
            break;
         case A_UVAL16:
            tdata = UINT2NUM( *(  unsigned short * )value->buffer );
            break;
         case A_VAL8:
            tdata = CHR2FIX( *( unsigned char * )value->buffer );
            break;
         case A_UVAL8:
            tdata = CHR2FIX( *( unsigned char * )value->buffer );
            break;
         default:
            rb_raise( rb_eTypeError, "Invalid Data Type" );
            tdata = Qnil;
            break;
         }
      }

   return tdata;
   }

/*
 * call-seq:
 *    ads_initialize_interface( VALUE imp_drh ) -> int result
 *
 *  Initializes the AdvantageInterface object and loads the DLL dynamically.
 *
 *  This function attempts to load the Advantage C API DLL dynamically and
 *  looks up all the entry points of the DLL.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An API structure to initialize.
 *
 *  <b>Returns</b>:
 *  - <tt>result</tt>: <tt>1</tt> on successful initialization, <tt>0</tt> on failure.
 *
 */
static VALUE
static_API_ads_initialize_interface( VALUE module, VALUE imp_drh )
   {
   imp_drh_st* s_imp_drh;
   int result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );

   result = ads_initialize_interface(  &( s_imp_drh->api ), NULL  );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *    ads_finalize_interface( VALUE imp_drh ) -> nil
 *
 *  Finalize and free resources associated with the Advantage C API DLL.
 *
 *  This function will unload the library and uninitialize the supplied
 *  AdvantageInterface structure.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_API_ads_finalize_interface( VALUE module, VALUE imp_drh )
   {
   imp_drh_st* s_imp_drh;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );

   ads_finalize_interface( &( s_imp_drh->api ) );

   free( &( s_imp_drh->api ) );

   return(  Qnil  );
   }

static VALUE
static_AdvantageInterface_alloc( VALUE class )
   {
   imp_drh_st *imp_drh = NULL;
   VALUE          tdata;

   imp_drh = malloc(  sizeof( imp_drh_st )  );
   memset(  imp_drh, 0, sizeof( imp_drh_st ) );

   tdata = Data_Wrap_Struct( class, 0, 0, imp_drh );
   return tdata;
   }

/*
 * call-seq:
 *   ads_init( VALUE imp_drh ) -> [VALUE result, VALUE version]
 *
 *  Initializes the interface.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success, <tt>0</tt> on failure.
 *  - <tt>VALUE version</tt>: The maximum API version that is supported.
 *
 */
static VALUE
static_AdvantageInterface_ads_init( VALUE imp_drh )
   {
   imp_drh_st* s_imp_drh;
   UNSIGNED32 result;
   UNSIGNED32 s_version_available;
   VALUE multi_result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );

   multi_result = rb_ary_new(  );

   if( &( s_imp_drh->api ) == NULL )
      {
      rb_ary_push( multi_result, INT2NUM( 0 ) );
      rb_ary_push( multi_result, Qnil  );
      }
   else
      {
      result = s_imp_drh->api.ads_init( ( UNSIGNED8* )"RUBY", 1 , &s_version_available  );
      rb_ary_push( multi_result, INT2NUM( result ) );
      rb_ary_push( multi_result, INT2NUM( s_version_available ) );
      }

   return(  multi_result  );
   }

/*
 * call-seq:
 *   ads_new_connection( VALUE imp_drh ) -> VALUE connection
 *
 *  Creates a connection object.
 *
 *  An API connection object needs to be created before a database connection
 *  is established. Errors can be retrieved from the connection object. Only
 *  one request can be processed on a connection at a time.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE connection</tt>: A connection object.
 *
 */
static VALUE
static_AdvantageInterface_ads_new_connection( VALUE imp_drh )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* ptr;
   VALUE tdata;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   ptr = s_imp_drh->api.ads_new_connection(  );

   tdata = Data_Wrap_Struct( cA_ads_connection, 0, 0, ptr );

   return( tdata );
   }

/*
 * call-seq:
 *   ads_client_version( VALUE imp_drh ) -> [VALUE verstr]
 *
 *  Retrieves the client version as a string.
 *
 *  This function can be used to retrieve the client version.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> --  an initialized API structure to finalize
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE verstr</tt>: The client version string.
 *
 */

static VALUE
static_AdvantageInterface_ads_client_version( VALUE imp_drh )
   {
   imp_drh_st* s_imp_drh;
   char s_buffer[255];

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );

   s_imp_drh->api.ads_client_version( ( UNSIGNED8* )s_buffer, 255 );

   return( rb_str_new2( s_buffer ) );
   }

/*
 * call-seq:
 *   ads_connect( VALUE imp_drh, VALUE ads_conn, VALUE str ) -> VALUE result
 *
 *  Creates a connection object.
 *
 *  An API connection object needs to be created before a database connection
 *  is established. Errors can be retrieved from the connection object. Only
 *  one request can be processed on a connection at a time.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was created by ads_new_connection(  ).
 *  - <tt>VALUE str</tt> -- A connection string.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success, <tt>0</tt> on failure.
 *
 */
static VALUE
static_AdvantageInterface_ads_connect( VALUE imp_drh, VALUE ads_conn, VALUE str )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   char* s_str;
   UNSIGNED32   result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   s_str = StringValueCStr(  str  );

   result = s_imp_drh->api.ads_connect(  s_ads_conn, ( UNSIGNED8* )s_str  );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_disconnect( VALUE imp_drh, VALUE ads_conn ) -> nil
 *
 *  Disconnect an already established connection.
 *
 *  This function disconnects the connection. Any
 *  uncommitted transactions will be rolled back.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_AdvantageInterface_ads_disconnect( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );


   s_imp_drh->api.ads_disconnect(  s_ads_conn  );

   return(  Qnil  );
   }

/*
 * call-seq:
 *   ads_free_connection( VALUE imp_drh, VALUE ads_conn ) -> nil
 *
 *  Frees the resources associated with a connection object.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was disconnected by ads_disconnect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_AdvantageInterface_ads_free_connection( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );


   s_imp_drh->api.ads_free_connection(  s_ads_conn  );

   return(  Qnil  );
   }

/*
 * call-seq:
 *   ads_fini( VALUE imp_drh ) -> nil
 *
 *  Finalizes the interface.
 *
 *  Thus function frees any resources allocated by the API.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_AdvantageInterface_ads_fini( VALUE imp_drh )
   {
   imp_drh_st* s_imp_drh;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );

   s_imp_drh->api.ads_fini(  );

   return(  Qnil  );
   }

/*
 * call-seq:
 *   ads_error( VALUE imp_drh, VALUE ads_conn ) -> [VALUE result, VALUE errstr]
 *
 *  Retrieves the last error code and message.
 *
 *  This function can be used to retrieve the last error code and message
 *  stored in the connection object.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: The last error code. Positive values are warnings, negative values are errors, and <tt>0</tt> is success.
 *  - <tt>VALUE errstr</tt>: The error message corresponding to the error code.
 *
 */

static VALUE
static_AdvantageInterface_ads_error( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   char s_buffer[255];
   UNSIGNED32 result;
   VALUE multi_result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   result = s_imp_drh->api.ads_error( s_ads_conn, ( UNSIGNED8* )s_buffer, 255 );

   multi_result = rb_ary_new(  );

   rb_ary_push( multi_result, INT2NUM( result ) );
   rb_ary_push( multi_result, rb_str_new2( s_buffer ) );

   return(  multi_result  );
   }

/*
 * call-seq:
 *   ads_execute_immediate( VALUE imp_drh, VALUE ads_conn, VALUE sql ) -> VALUE result
 *
 *  Executes a SQL statement immediately without returning a result set.
 *
 *  This function executes the specified SQL statement immediately. It is
 *  useful for SQL statements that do not return a result set.
 *
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *  - <tt>VALUE sql</tt> -- A SQL query string.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success, <tt>0</tt> on failure.
 *
 */
static VALUE
static_AdvantageInterface_ads_execute_immediate( VALUE imp_drh, VALUE ads_conn, VALUE sql )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   char* s_sql;
   UNSIGNED32 result;

   s_sql = StringValueCStr(  sql  );

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   result = s_imp_drh->api.ads_execute_immediate( s_ads_conn, ( UNSIGNED8* )s_sql );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_execute_direct( VALUE imp_drh, VALUE ads_conn, VALUE sql ) -> VALUE resultset
 *
 *  Executes a SQL statement and possibly returns a result set.
 *
 *  This function executes the SQL statement specified by the string argument.
 *  This function is suitable if you want to prepare and then execute a
 *  statement, and can be used instead of calling ads_prepare(  ) followed
 *  by ads_execute(  ).
 *
 *  This function cannot be used for executing a SQL statement with
 *  parameters.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *  - <tt>VALUE sql</tt> -- A SQL query string.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: A query result set if successful, nil if failed.
 *
 */
static VALUE
static_AdvantageInterface_ads_execute_direct( VALUE imp_drh, VALUE ads_conn, VALUE sql )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   ADSHANDLE resultset = 0;
   char* s_sql;
   VALUE tdata;

   s_sql = StringValueCStr(  sql  );

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   resultset = s_imp_drh->api.ads_execute_direct( s_ads_conn, ( UNSIGNED8* )s_sql );

   if( resultset )
      {
      tdata = INT2NUM( resultset );
      }
   else
      {
      tdata = Qnil;
      }

   return( tdata );
   }

/*
 * call-seq:
 *   ads_num_cols( VALUE imp_drh, VALUE ads_stmt ) -> VALUE num_cols
 *
 *  Returns number of columns in the result set.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE num_cols</tt>: The number of columns in the result set or <tt>-1</tt> on a failure.
 *
 */
static VALUE
static_AdvantageInterface_ads_num_cols( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_num_cols( s_stmt );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_num_rows( VALUE imp_drh, VALUE ads_stmt ) -> VALUE num_rows
 *
 *  Returns number of rows in the result set.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> --  An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE num_rows</tt>: The number of rows in the result set or <tt>-1</tt> on a failure.
 *
 */
static VALUE
static_AdvantageInterface_ads_num_rows( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_num_rows( s_stmt );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_get_column( VALUE imp_drh, VALUE ads_stmt, VALUE col_num ) -> [VALUE result, VALUE column_value]
 *
 *  Retrieves the data fetched for the specified column.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *  - <tt>VALUE col_num</tt> -- The number of the column to be retrieved. A column number is between 0 and ads_num_cols(  ) - 1.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE column_value</tt>: The value of the column. nil is returned if the value was NULL.
 *
 */
static VALUE
static_AdvantageInterface_ads_get_column( VALUE imp_drh, VALUE ads_stmt, VALUE col_num )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 s_col_num;
   a_ads_data_value value;
   UNSIGNED32 result;
   VALUE multi_result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_stmt );
   s_stmt = NUM2ULONG( ads_stmt );
   s_col_num = NUM2INT( col_num );

   result = s_imp_drh->api.ads_get_column( s_stmt, s_col_num, &value  );

   multi_result = rb_ary_new(  );
   rb_ary_push( multi_result, INT2NUM( result ) );

   if( !result )
      {
      rb_ary_push( multi_result, Qnil );
      }
   else
      {
      if( *value.is_null )
         {
         rb_ary_push( multi_result, Qnil );
         }
      else
         {
         rb_ary_push( multi_result, C2RB( &value ) );
         }
      }

   return(  multi_result  );
   }

/*
 * call-seq:
 *   ads_fetch_next( VALUE imp_drh, VALUE ads_stmt ) -> VALUE result
 *
 *  Fetches the next row from the result set.
 *
 *  This function fetches the next row from the result set. When the result
 *  object is first created, the current row pointer is set to point before
 *  the first row ( that is, row 0 ).
 *  This function advances the row pointer first and then fetches the data
 *  at the new row.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on successful fetch, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_fetch_next( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_fetch_next( s_stmt );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_get_column_info( VALUE imp_drh, VALUE ads_stmt, VALUE col_num ) -> [VALUE result, VALUE col_num, VALUE name, VALUE type, VALUE max_size]
 *
 *  Fetches the next row from the result set.
 *
 *  This function fetches the next row from the result set. When the result
 *  object is first created, the current row pointer is set to point before
 *  the first row ( that is, row 0 ).
 *  This function advances the row pointer first and then fetches the data
 *  at the new row.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *  - <tt>VALUE col_num</tt> -- The number of the column to be retrieved. A column number is between 0 and ads_num_cols(  ) - 1.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success. Returns <tt>0</tt> if the column index is out of range, or if the statement does not return a result set.
 *  - <tt>VALUE col_num</tt>: The number of the column retrieved.
 *  - <tt>VALUE name</tt>: The name of the column.
 *  - <tt>VALUE type</tt>: The type of the column data.
 *  - <tt>VALUE native_type</tt>: The Advantage native type of the column data.
 *  - <tt>VALUE precision</tt>: The precision of the column.
 *  - <tt>VALUE scale</tt>: The scale of the column.
 *  - <tt>VALUE max_size</tt>: The maximum size a data value in this column can take.
 *  - <tt>VALUE nullable</tt>: The nullability of the column.
 *
 */
static VALUE
static_AdvantageInterface_ads_get_column_info( VALUE imp_drh, VALUE ads_stmt, VALUE col_num )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 s_col_num;
   a_ads_column_info info;
   UNSIGNED32 result;
   VALUE multi_result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_stmt );
   s_stmt = NUM2ULONG( ads_stmt );
   s_col_num = NUM2INT( col_num );

   result = s_imp_drh->api.ads_get_column_info( s_stmt, s_col_num, &info  );

   multi_result = rb_ary_new(  );
   rb_ary_push( multi_result, INT2NUM( result ) );
   rb_ary_push( multi_result, col_num  );
   rb_ary_push( multi_result, rb_str_new2( ( char* )info.name ) );
   rb_ary_push( multi_result, INT2NUM( info.type ) );
   rb_ary_push( multi_result, INT2NUM( info.native_type ) );
   rb_ary_push( multi_result, INT2NUM( info.precision ) );
   rb_ary_push( multi_result, INT2NUM( info.scale ) );
   rb_ary_push( multi_result, INT2NUM( info.max_size ) );
   rb_ary_push( multi_result, INT2NUM( info.nullable ) );

   return(  multi_result  );
   }

/*
 * call-seq:
 *   AdsBeginTransaction( VALUE imp_drh, VALUE ads_conn ) -> VALUE result
 *
 *  Starts a transaction on the current connection.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on successful begining a transaction, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_AdsBeginTransaction( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   result = s_imp_drh->api.AdsBeginTransaction( s_ads_conn->hConnect );
   if( result == 0 )
      return(  INT2NUM( 1 )  );
   else
      return(   INT2NUM( 0 )  );
   }

/*
 * call-seq:
 *   ads_commit( VALUE imp_drh, VALUE ads_conn ) -> VALUE result
 *
 *  Commits the current transaction.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on successful commit, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_commit( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   result = s_imp_drh->api.ads_commit( s_ads_conn );

   return(  INT2NUM( result )  );
   }


/*
 * call-seq:
 *   ads_rollback( VALUE imp_drh, VALUE ads_conn ) -> VALUE result
 *
 *  Rolls back the current transaction.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on successful rollback, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_rollback( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   result = s_imp_drh->api.ads_rollback( s_ads_conn );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_prepare( VALUE imp_drh, VALUE ads_conn, VALUE sql ) -> VALUE stmt
 *
 *  Prepares a SQL statement.
 *
 *  This function prepares the supplied SQL string. Execution does not
 *  happen until ads_execute(  ) is called. The returned statement object
 *  should be freed using ads_free_stmt(  ).
 *
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *  - <tt>VALUE sql</tt> -- SQL query to prepare.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE stmt</tt>: A statement object, or nil on failure. The statement object can be used by ads_execute(  ) to execute the statement.
 *
 */
static VALUE
static_AdvantageInterface_ads_prepare( VALUE imp_drh, VALUE ads_conn, VALUE sql )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   ADSHANDLE s_stmt = 0;
   char* s_sql;
   VALUE tdata;

   s_sql = StringValueCStr(  sql  );

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   //EJS Passing FALSE for isUnicode
   s_stmt = s_imp_drh->api.ads_prepare( s_ads_conn, ( UNSIGNED8* )s_sql, '\0' );

   if( s_stmt )
      {
      tdata = INT2NUM(  s_stmt  );
      }
   else
      {
      tdata = Qnil;
      }

   return( tdata );
   }

/*
 * call-seq:
 *   ads_free_stmt( VALUE imp_drh, VALUE ads_stmt ) -> nil
 *
 *  Frees resources associated with a prepared statement object.
 *
 *  This function frees the resources associated with a prepared statement
 *  object.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_AdvantageInterface_ads_free_stmt( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   int i;
   int number_of_params = 0;
   a_ads_bind_param_info bind_info;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   number_of_params = s_imp_drh->api.ads_num_params( s_stmt );

   for( i = 0; i < number_of_params; i++ )
      {
      if( s_imp_drh->api.ads_get_bind_param_info( s_stmt, i, &bind_info ) )
         {
         // We don't free bind_info.name as it's not allocated
         // if ( bind_info.name ) {free ( bind_info.name );}

         if( bind_info.input_value.is_null )
            {
            free( bind_info.input_value.is_null );
            }
         if( bind_info.input_value.length )
            {
            free( bind_info.input_value.length );
            }
         if( bind_info.input_value.buffer )
            {
            free( bind_info.input_value.buffer );
            }

         if( bind_info.output_value.is_null )
            {
            free( bind_info.output_value.is_null );
            }
         if( bind_info.output_value.length )
            {
            free( bind_info.output_value.length );
            }
         if( bind_info.output_value.buffer )
            {
            free( bind_info.output_value.buffer );
            }
         }
      }

   s_imp_drh->api.ads_free_stmt( s_stmt );

   return(  Qnil  );
   }

/*
 * call-seq:
 *   ads_reset( VALUE imp_drh, VALUE ads_stmt ) -> VALUE result
 *
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on successful execution, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_reset( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_reset( s_stmt );

   return( INT2NUM( result ) );
   }


/*
 * call-seq:
 *   ads_execute( VALUE imp_drh, VALUE ads_stmt ) -> VALUE result
 *
 *  Executes a prepared statement.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was created by ads_prepare(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on successful execution, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_execute( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   //printf(  "CEXT s_ads_stmt: %d \n", s_stmt );
   result = s_imp_drh->api.ads_execute( s_stmt );
   /*if (  result != AE_SUCCESS  )
      return ( 0 );
   else
      return ( 1 );*/
   return( INT2NUM( result ) );
   }

/*
 * call-seq:
 *   ads_affected_rows( VALUE imp_drh, VALUE ads_stmt ) -> VALUE result
 *
 *  Returns the number of rows affected by execution of the prepared
 *  statement.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement that was prepared and executed successfully with no result set returned.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: The number of rows affected or <tt>-1</tt> on failure.
 *
 */
static VALUE
static_AdvantageInterface_ads_affected_rows( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_affected_rows( s_stmt );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_describe_bind_param( VALUE imp_drh, VALUE ads_stmt, VALUE index ) -> [VALUE result, VALUE bind_param]
 *
 *  Describes the bind parameters of a prepared statement.
 *
 *  This function allows the caller to determine information about parameters
 *  to a prepared statement. Depending on the type of the prepared statement
 *  ( call to stored procedure or a DML ), only some information will be
 *  provided. The information that will always be provided is the direction
 *  of the parameters ( input, output, or input-output ).
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was returned from ads_prepare(  ).
 *  - <tt>VALUE index</tt> -- The index of the parameter. This should be a number between 0 and ads_num_params(  )-  1.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success or <tt>0</tt> on failure.
 *  - <tt>VALUE bind_param</tt>: The described param object.
 *
 */
static VALUE
static_AdvantageInterface_ads_describe_bind_param( VALUE imp_drh, VALUE ads_stmt, VALUE index )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   a_ads_bind_param* s_ads_bind_param;
   UNSIGNED32 result;
   UNSIGNED32 s_index;
   VALUE tdata;
   VALUE multi_result;

   s_ads_bind_param = malloc( sizeof( a_ads_bind_param ) );
   memset(  s_ads_bind_param, 0, sizeof( a_ads_bind_param )  );

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   s_stmt = NUM2ULONG( ads_stmt );
   s_index = NUM2INT( index );

   result = s_imp_drh->api.ads_describe_bind_param( s_stmt, s_index, s_ads_bind_param );

   //FIXME handle failed result

   multi_result = rb_ary_new(  );

   rb_ary_push( multi_result, INT2NUM( result ) );

   tdata = Data_Wrap_Struct( cA_ads_bind_param, 0, 0, s_ads_bind_param );
   rb_ary_push( multi_result, tdata );

   return(  multi_result  );
   }

/*
 * call-seq:
 *   ads_bind_param( VALUE imp_drh, VALUE ads_stmt, VALUE index, VALUE ads_bind_param ) -> VALUE result
 *
 *  Binds a user supplied buffer as a parameter to the prepared statement.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was returned from ads_prepare(  ).
 *  - <tt>VALUE index</tt> -- The index of the parameter. This should be a number between 0 and ads_num_params(  ) - 1.
 *  - <tt>VALUE ads_bind_param</tt> -- A filled bind object retrieved from ads_describe_bind_param(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success or <tt>0</tt> on failure.
 *
 */
static VALUE
static_AdvantageInterface_ads_bind_param( VALUE imp_drh, VALUE ads_stmt, VALUE index, VALUE ads_bind_param  )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   a_ads_bind_param* s_ads_bind_param;
   UNSIGNED32 result;
   UNSIGNED32 s_index;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );
   Data_Get_Struct( ads_bind_param, a_ads_bind_param, s_ads_bind_param );
   s_index = NUM2INT( index );

   result = s_imp_drh->api.ads_bind_param( s_stmt, s_index, s_ads_bind_param );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_get_bind_param_info( VALUE imp_drh, VALUE ads_stmt, VALUE index ) -> [VALUE result, VALUE bind_param]
 *
 *  Gets bound parameter info.
 *
 *  This function retrieves information about the parameters that were
 *  bound using ads_bind_param(  ).
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was returned from ads_prepare(  ).
 *  - <tt>VALUE index</tt> -- The index of the parameter. This should be a number between 0 and ads_num_params(  ) - 1.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> on success or <tt>0</tt> on failure.
 *  - <tt>VALUE bind_param</tt>: The described param object.
 *
 */
static VALUE
static_AdvantageInterface_ads_get_bind_param_info( VALUE imp_drh, VALUE ads_stmt, VALUE index )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   a_ads_bind_param_info s_ads_bind_param_info;
   UNSIGNED32 result;
   UNSIGNED32 s_index;
   VALUE tdata;
   VALUE multi_result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   s_stmt = NUM2ULONG( ads_stmt );
   s_index = NUM2INT( index );

   result = s_imp_drh->api.ads_get_bind_param_info( s_stmt, s_index, &s_ads_bind_param_info );

   //FIXME handle failed result
   multi_result = rb_ary_new(  );

   rb_ary_push( multi_result, INT2NUM( result ) );

   // FIXME: Is this safe to be on the stack?
   tdata = Data_Wrap_Struct( cA_ads_bind_param_info, 0, 0, &s_ads_bind_param_info );
   rb_ary_push( multi_result, tdata );

   return(  multi_result  );
   }

/*
 * call-seq:
 *   ads_num_params( VALUE imp_drh, VALUE ads_stmt ) -> VALUE result
 *
 *  Returns the number of parameters that are expected for a prepared
 *  statement.
 *
 *  This function retrieves information about the parameters that were bound
 *  using ads_bind_param(  ).
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was returned from ads_prepare(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: The number of parameters that are expected. <tt>-1</tt> if the ads_stmt object is not valid.
 *
 */
static VALUE
static_AdvantageInterface_ads_num_params( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_num_params( s_stmt );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_get_next_result( VALUE imp_drh, VALUE ads_stmt ) -> VALUE result
 *
 *  Advances to the next result set in a multiple result set query.
 *
 *  If a query ( such as a call to a stored procedure ) returns multiple result
 *  sets, then this function advances from the current result set to the next.
 *
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was executed by ads_execute(  ) or ads_execute_direct(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> if was successfully able to advance to the next result set, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_get_next_result( VALUE imp_drh, VALUE ads_stmt )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );

   result = s_imp_drh->api.ads_get_next_result( s_stmt );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_fetch_absolute( VALUE imp_drh, VALUE ads_stmt, VALUE offset ) -> VALUE result
 *
 *  Fetches data at a specific row number in the result set.
 *
 *  This function moves the current row in the result set to the row number
 *  specified and fetches the data at that row.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_stmt</tt> -- A statement object that was executed by ads_execute(  ) or ads_execute_direct(  ).
 *  - <tt>VALUE offset</tt> -- The row number to be fetched. The first row is <tt>1</tt>, the last row is <tt>-1</tt>.
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE result</tt>: <tt>1</tt> if the fetch was successfully, <tt>0</tt> otherwise.
 *
 */
static VALUE
static_AdvantageInterface_ads_fetch_absolute( VALUE imp_drh, VALUE ads_stmt, VALUE offset )
   {
   imp_drh_st* s_imp_drh;
   ADSHANDLE s_stmt;
   UNSIGNED32  s_offset;
   UNSIGNED32 result;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   //Data_Get_Struct( ads_stmt, ADSHANDLE, s_ads_stmt );
   s_stmt = NUM2ULONG( ads_stmt );
   s_offset = NUM2INT( offset );
   result = s_imp_drh->api.ads_fetch_absolute( s_stmt, s_offset );

   return(  INT2NUM( result )  );
   }

/*
 * call-seq:
 *   ads_sqlstate( VALUE imp_drh, VALUE ads_conn ) -> VALUE sqlstate_str
 *
 *  Retrieves the current SQL state.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE sqlstate_str</tt>: The SQL State.
 *
 */
static VALUE
static_AdvantageInterface_ads_sqlstate( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;
   UNSIGNED32 result;
   char   s_buffer[255];

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   result = s_imp_drh->api.ads_sqlstate( s_ads_conn, ( UNSIGNED8* )s_buffer, sizeof( s_buffer ) );

   if( result )
      return(  rb_str_new( s_buffer, strlen( s_buffer ) ) );
   else
      return Qnil;
   }

/*
 * call-seq:
 *   ads_clear_error( VALUE imp_drh, VALUE ads_conn ) -> nil
 *
 *  Clears the last stored error code.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE imp_drh</tt> -- An initialized API structure to finalize.
 *  - <tt>VALUE ads_conn</tt> -- A connection object that was connected by ads_connect(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>:.
 *
 */
static VALUE
static_AdvantageInterface_ads_clear_error( VALUE imp_drh, VALUE ads_conn )
   {
   imp_drh_st* s_imp_drh;
   a_ads_connection* s_ads_conn;

   Data_Get_Struct( imp_drh, imp_drh_st, s_imp_drh );
   Data_Get_Struct( ads_conn, a_ads_connection, s_ads_conn );

   s_imp_drh->api.ads_clear_error( s_ads_conn );

   return(  Qnil  );
   }

/*
 * call-seq:
 *   get_name( VALUE bind ) -> VALUE name
 *
 *  Gets the name of the bound parameter.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE name</tt>: The bound variable's name.
 *
 */
static VALUE
static_Bind_get_name( VALUE bind )
   {
   a_ads_bind_param* s_bind;
   Data_Get_Struct( bind, a_ads_bind_param, s_bind );
   return( rb_str_new2( ( char* )s_bind->name ) );
   }

/*
 * call-seq:
 *   set_value( VALUE bind, VALUE val ) -> nil
 *
 *  Sets the value of a bind parameter.
 *
 *  This function is used to bind a Ruby VALUE to a given parameter in a
 *  prepared statement. If the bind_direction is INPUT only, the type INPUT
 *  will be bound based on the RUBY type:
 *
 *    +-------------+------------+
 *    | Ruby Type   |  Bind Type |
 *    +-------------+------------+
 *    | T_STRING   =>  A_STRING  |
 *    | T_FIXNUM   =>  A_VAL_32  |
 *    | T_BIGNUM   =>  A_DOUBLE  |
 *    | T_FLOAT    =>  A_DOUBLE  |
 *    | T_STRING   =>  A_STRING  |
 *    | T_NIL      => isnull = 1 |
 *    +--------------------------+
 *
 *  If the bind direction is OUTPUT, instead use the DBCAPI type to set the
 *  size of the buffer.
 *
 *  In the case of INOUT parameters, it is the application's job to call this
 *  method twice, once to bind the INPUT, and once to bind the OUTPUT.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *  - <tt>VALUE val</tt> -- The value to bind into the bound variable.
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 */
static VALUE
static_Bind_set_value( VALUE bind, VALUE val )
   {
   a_ads_bind_param* s_bind;
   int length;

   Data_Get_Struct( bind, a_ads_bind_param, s_bind );

   // FIXME: use Ruby's allocation routines?
   s_bind->value.is_null = malloc( sizeof( UNSIGNED32 ) );
   *s_bind->value.is_null = 0;

   if( s_bind->direction == DD_INPUT )
      {
      switch( TYPE( val ) )
         {
         case T_STRING:
            s_bind->value.length  = malloc( sizeof( size_t ) );
            length = RSTRING_LEN( val );
            *s_bind->value.length = length;
            s_bind->value.buffer = malloc( length );
            memcpy( s_bind->value.buffer, RSTRING_PTR( val ), length );
            s_bind->value.type = A_STRING;
            break;
         case T_FIXNUM:
            if( sizeof( void* ) == 4 ) //32-bit
               {
               s_bind->value.length  = malloc( sizeof( size_t ) );
               s_bind->value.buffer = malloc( sizeof( int ) );
               *( ( int* )s_bind->value.buffer ) = FIX2INT( val );
               s_bind->value.type = A_VAL32;
               length = 1;
               *s_bind->value.length = length;
               }
            else //64-bit
               {
               s_bind->value.length  = malloc( sizeof( size_t ) );
               s_bind->value.buffer = malloc( sizeof( long ) );
               *( ( long* )s_bind->value.buffer ) = FIX2LONG( val );
               s_bind->value.type = A_VAL64;
               length = 1;
               *s_bind->value.length = length;
               }
            break;
         case T_BIGNUM:
            s_bind->value.length  = malloc( sizeof( size_t ) );
            s_bind->value.buffer = malloc( sizeof( LONG_LONG ) );
            *( ( LONG_LONG* )s_bind->value.buffer ) = rb_num2ll( val );
            s_bind->value.type = A_VAL64;
            length = 1;
            *s_bind->value.length = length;
            break;
         case T_FLOAT:
            s_bind->value.length  = malloc( sizeof( size_t ) );
            s_bind->value.buffer = malloc( sizeof( double ) );
            *( ( double* )s_bind->value.buffer ) = NUM2DBL( val );
            s_bind->value.type = A_DOUBLE;
            length = 1;
            *s_bind->value.length = length;
            break;
         case T_FALSE:
            s_bind->value.length  = malloc( sizeof( size_t ) );
            s_bind->value.buffer = malloc( sizeof( short ) );
            *( ( int* )s_bind->value.buffer ) = ADS_FALSE;
            s_bind->value.type = A_UVAL16;
            length = 1;
            *s_bind->value.length = length;
            break;
         case T_TRUE:
            s_bind->value.length  = malloc( sizeof( size_t ) );
            s_bind->value.buffer = malloc( sizeof( short ) );
            *( ( int* )s_bind->value.buffer ) = ADS_TRUE;
            s_bind->value.type = A_UVAL16;
            length = 1;
            *s_bind->value.length = length;
            break;
         case T_NIL:
            s_bind->value.length  = malloc( sizeof( size_t ) );
            *s_bind->value.is_null = 1;
            s_bind->value.buffer = malloc( sizeof( int ) );
            s_bind->value.type = A_VAL32;
            length = 1;
            *s_bind->value.length = length;
            break;
         default:
            rb_raise( rb_eTypeError, "Cannot convert type. Must be STRING, FIXNUM, BIGNUM, FLOAT, or NIL" );
            break;
         }
      }
   else
      {
      switch( s_bind->value.type )
         {
         case A_STRING:
            s_bind->value.buffer = malloc( s_bind->value.buffer_size );
            break;
         case A_DOUBLE:
            s_bind->value.buffer = malloc( sizeof( float ) );
            break;
         case A_VAL64:
         case A_UVAL64:
            s_bind->value.buffer = malloc( sizeof( LONG_LONG ) );
            break;
         case A_VAL32:
         case A_UVAL32:
            s_bind->value.buffer = malloc( sizeof( int ) );
            break;
         case A_VAL16:
         case A_UVAL16:
            s_bind->value.buffer = malloc( sizeof( short ) );
            break;
         case A_VAL8:
         case A_UVAL8:
            s_bind->value.buffer = malloc( sizeof( char ) );
            break;
         default:
            rb_raise( rb_eTypeError, "Type unknown" );
            break;
         }
      }
   return( Qnil );
   }

/*
 * call-seq:
 *   set_direction( VALUE bind, VALUE direction ) -> nil
 *
 *  Sets the direction of the bound parameter before binding.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *  - <tt>VALUE direction</tt> -- The direction of the binding variable.
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_Bind_set_direction( VALUE bind, VALUE direction )
   {
   a_ads_bind_param* s_bind;

   Data_Get_Struct( bind, a_ads_bind_param, s_bind );

   s_bind->direction = NUM2CHR( direction );

   return( Qnil );
   }

/*
 * call-seq:
 *   set_buffer_size( VALUE bind, VALUE size ) -> nil
 *
 *  Sets the buffer size of INOUT and OUT parameters for string and binary
 *  data.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *  - <tt>VALUE size</tt> -- The size of the buffer to hold the INOUT or OUT parameter.
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_Bind_set_buffer_size( VALUE bind, VALUE size )
   {
   a_ads_bind_param* s_bind;

   Data_Get_Struct( bind, a_ads_bind_param, s_bind );

   s_bind->value.buffer_size = NUM2INT( size );

   return( Qnil );
   }

/*
 * call-seq:
 *   get_direction( VALUE bind ) -> VALUE direction
 *
 *  Gets the direction of the bound parameter.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE name</tt>: The direction of the bound parameter.
 *
 */
static VALUE
static_Bind_get_direction( VALUE bind )
   {
   a_ads_bind_param* s_bind;
   Data_Get_Struct( bind, a_ads_bind_param, s_bind );

   return( CHR2FIX( s_bind->direction ) );
   }

/*
 * call-seq:
 *   finish( VALUE bind ) -> nil
 *
 *  Frees the resources associated with the bound parameter.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>nil</tt>.
 *
 */
static VALUE
static_Bind_finish( VALUE bind )
   {
   a_ads_bind_param* s_bind;

   Data_Get_Struct( bind, a_ads_bind_param, s_bind );

   // FIXME: use Ruby's allocation routines?
   if( s_bind )
      {
      free( s_bind );
      };

   return( Qnil );
   }

/*
 * call-seq:
 *  get_info_direction( VALUE bind ) -> VALUE direction
 *
 *  Gets the name of the bound parameter info object.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter info retrieved from ads_describe_bind_param_info(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE direction</tt>: The bound variable's direction.
 *
 */
static VALUE
static_Bind_get_info_direction( VALUE bind )
   {
   a_ads_bind_param_info* s_bind;
   Data_Get_Struct( bind, a_ads_bind_param_info, s_bind );

   return( CHR2FIX( s_bind->direction ) );
   }

/*
 * call-seq:
 *   get_info_output( VALUE bind ) -> VALUE name
 *
 *  Gets the value of a bound parameter after execution.
 *
 *  <b>Parameters</b>:
 *  - <tt>VALUE bind</tt> -- Bound parameter retrieved from ads_describe_bind_param(  ).
 *
 *  <b>Returns</b>:
 *  - <tt>VALUE name</tt>: The bound variable value.
 *
 */
static VALUE
static_Bind_get_info_output( VALUE bind )
   {
   a_ads_bind_param_info* s_bind;
   Data_Get_Struct( bind, a_ads_bind_param_info, s_bind );
   return( C2RB( &s_bind->output_value ) );
   }

/*
 *
 */
void Init_advantage(  )
   {
   // Define top leve 'Advantage' module
   mAdvantage = rb_define_module(  "Advantage"  );

   // Define a sub-module name 'API' under Advantage.
   // In Ruby, this is accessed as Advantage::API
   mAPI = rb_define_module_under(  mAdvantage, "API"  );

   // Define ads interface functions
   rb_define_module_function(  mAPI, "ads_initialize_interface", static_API_ads_initialize_interface, 1  );
   rb_define_module_function(  mAPI, "ads_finalize_interface", static_API_ads_finalize_interface, 1  );
   rb_define_const( mAPI, "VERSION", rb_str_new2( VERSION ) );


   // Define interface classes under the Advantage module
   cAdvantageInterface = rb_define_class_under(  mAdvantage, "AdvantageInterface", rb_cObject );
   rb_define_alloc_func( cAdvantageInterface, static_AdvantageInterface_alloc );

   // Define all of the DBCAPI functions as methods under an interface instance
   rb_define_method( cAdvantageInterface, "ads_init", static_AdvantageInterface_ads_init, 0 );
   rb_define_method( cAdvantageInterface, "ads_new_connection", static_AdvantageInterface_ads_new_connection, 0 );
   rb_define_method( cAdvantageInterface, "ads_client_version", static_AdvantageInterface_ads_client_version, 0 );
   rb_define_method( cAdvantageInterface, "ads_connect", static_AdvantageInterface_ads_connect, 2 );
   rb_define_method( cAdvantageInterface, "ads_disconnect", static_AdvantageInterface_ads_disconnect, 1 );
   rb_define_method( cAdvantageInterface, "ads_free_connection", static_AdvantageInterface_ads_free_connection, 1 );
   rb_define_method( cAdvantageInterface, "ads_fini", static_AdvantageInterface_ads_fini, 0 );
   rb_define_method( cAdvantageInterface, "ads_error", static_AdvantageInterface_ads_error, 1 );
   rb_define_method( cAdvantageInterface, "ads_execute_immediate", static_AdvantageInterface_ads_execute_immediate, 2 );
   rb_define_method( cAdvantageInterface, "ads_execute_direct", static_AdvantageInterface_ads_execute_direct, 2 );
   rb_define_method( cAdvantageInterface, "ads_num_cols", static_AdvantageInterface_ads_num_cols, 1 );
   rb_define_method( cAdvantageInterface, "ads_num_rows", static_AdvantageInterface_ads_num_rows, 1 );
   rb_define_method( cAdvantageInterface, "ads_get_column", static_AdvantageInterface_ads_get_column, 2 );
   rb_define_method( cAdvantageInterface, "ads_fetch_next", static_AdvantageInterface_ads_fetch_next, 1 );
   rb_define_method( cAdvantageInterface, "ads_get_column_info", static_AdvantageInterface_ads_get_column_info, 2 );
   rb_define_method( cAdvantageInterface, "AdsBeginTransaction", static_AdvantageInterface_AdsBeginTransaction, 1 );
   rb_define_method( cAdvantageInterface, "ads_commit", static_AdvantageInterface_ads_commit, 1 );
   rb_define_method( cAdvantageInterface, "ads_rollback", static_AdvantageInterface_ads_rollback, 1 );
   rb_define_method( cAdvantageInterface, "ads_prepare", static_AdvantageInterface_ads_prepare, 2 );
   rb_define_method( cAdvantageInterface, "ads_free_stmt", static_AdvantageInterface_ads_free_stmt, 1 );
   rb_define_method( cAdvantageInterface, "ads_reset", static_AdvantageInterface_ads_reset, 1 );
   rb_define_method( cAdvantageInterface, "ads_execute", static_AdvantageInterface_ads_execute, 1 );
   rb_define_method( cAdvantageInterface, "ads_affected_rows", static_AdvantageInterface_ads_affected_rows, 1 );
   rb_define_method( cAdvantageInterface, "ads_describe_bind_param", static_AdvantageInterface_ads_describe_bind_param, 2 );
   rb_define_method( cAdvantageInterface, "ads_bind_param", static_AdvantageInterface_ads_bind_param, 3 );
   rb_define_method( cAdvantageInterface, "ads_get_bind_param_info", static_AdvantageInterface_ads_get_bind_param_info, 2 );
   rb_define_method( cAdvantageInterface, "ads_num_params", static_AdvantageInterface_ads_num_params, 1 );
   rb_define_method( cAdvantageInterface, "ads_get_next_result", static_AdvantageInterface_ads_get_next_result, 1 );
   rb_define_method( cAdvantageInterface, "ads_fetch_absolute", static_AdvantageInterface_ads_fetch_absolute, 2 );
   rb_define_method( cAdvantageInterface, "ads_sqlstate", static_AdvantageInterface_ads_sqlstate, 1 );
   rb_define_method( cAdvantageInterface, "ads_clear_error", static_AdvantageInterface_ads_clear_error, 1 );

   // Define classes for accessing structures under Advantage module
   cA_ads_connection = rb_define_class_under ( mAdvantage, "a_ads_connection", rb_cObject );
   cA_ads_data_value = rb_define_class_under ( mAdvantage, "a_ads_data_value", rb_cObject );
   //cA_ads_stmt = rb_define_class_under ( mAdvantage, "ADSHANDLE", rb_cObject );
   cA_ads_bind_param = rb_define_class_under ( mAdvantage, "a_ads_bind_param", rb_cObject );
   cA_ads_bind_param_info = rb_define_class_under ( mAdvantage, "a_ads_bind_param_info", rb_cObject );


   // Define methods for obtaining bind_parameter fields
   rb_define_method( cA_ads_bind_param, "get_name", static_Bind_get_name, 0 );
   rb_define_method( cA_ads_bind_param, "set_value", static_Bind_set_value, 1 );
   rb_define_method( cA_ads_bind_param, "get_direction", static_Bind_get_direction, 0 );
   rb_define_method( cA_ads_bind_param, "set_direction", static_Bind_set_direction, 1 );
   rb_define_method( cA_ads_bind_param, "set_buffer_size", static_Bind_set_buffer_size, 1 );
   rb_define_method( cA_ads_bind_param, "finish", static_Bind_finish, 0 );

   // Define methods for obtaining bind_parameter_info fields
   rb_define_method( cA_ads_bind_param_info, "get_direction", static_Bind_get_info_direction, 0 );
   rb_define_method( cA_ads_bind_param_info, "get_output", static_Bind_get_info_output, 0 );
   }


