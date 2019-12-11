/* ====================================================
 *
*       Copyright 2008-2012 iAnywhere Solutions, Inc.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *
 * ====================================================
 */

#ifndef ADSCAPIDLL_H
#define ADSCAPIDLL_H

#include "ace.h"
#include "dbcapi.h"


#if defined( __cplusplus )
extern "C" {
#endif
   typedef UNSIGNED32            (WINAPI*ads_affected_rows_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*AdsBeginTransaction_func)( ADSHANDLE hConnect );
   typedef UNSIGNED32            (WINAPI*ads_bind_param_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param *param );
   typedef void                  (WINAPI*ads_cancel_func)( a_ads_connection *poConnect );
   typedef void                  (WINAPI*ads_clear_error_func)( a_ads_connection *poConnect );
   typedef UNSIGNED32            (WINAPI*ads_client_version_func)( UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength );
   typedef UNSIGNED32            (WINAPI*ads_commit_func)( a_ads_connection *poConnect );
   typedef UNSIGNED32            (WINAPI*ads_connect_func)( a_ads_connection *poConnect, UNSIGNED8 *pucConnectString );
   typedef UNSIGNED32            (WINAPI*ads_describe_bind_param_func)( ADSHANDLE ads_stmt, UNSIGNED32 index, a_ads_bind_param *params );
   typedef UNSIGNED32            (WINAPI*ads_disconnect_func)( a_ads_connection *poConnect );
   typedef UNSIGNED32            (WINAPI*ads_error_func)( a_ads_connection *poConnect, UNSIGNED8 *pucError, UNSIGNED32 ulLength );
   typedef UNSIGNED32            (WINAPI*ads_execute_func)( ADSHANDLE hStatement );
   typedef ADSHANDLE             (WINAPI*ads_execute_direct_func)( a_ads_connection *poConnect, UNSIGNED8 *pucSQL );
   typedef UNSIGNED32            (WINAPI*ads_execute_immediate_func)( a_ads_connection *poConnect, UNSIGNED8 *pucSQL );
   typedef UNSIGNED32            (WINAPI*ads_fetch_absolute_func)( ADSHANDLE hStatement, UNSIGNED32 ulRowNum );
   typedef UNSIGNED32            (WINAPI*ads_fetch_next_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*ads_fini_func)();
   typedef UNSIGNED32            (WINAPI*ads_free_connection_func)( a_ads_connection *poConnect );
   typedef UNSIGNED32            (WINAPI*ads_free_stmt_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*ads_get_bind_param_info_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param_info *info );
   typedef UNSIGNED32            (WINAPI*ads_get_column_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_data_value *buffer );
   typedef UNSIGNED32            (WINAPI*ads_get_column_info_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_column_info *buffer );
   typedef UNSIGNED32            (WINAPI*ads_get_data_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, UNSIGNED32 ulOffset, void *buffer, UNSIGNED32 ulLength );
   typedef UNSIGNED32            (WINAPI*ads_get_data_info_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_data_info *buffer );
   typedef UNSIGNED32            (WINAPI*ads_get_next_result_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*ads_init_func)( UNSIGNED8 *app_name, UNSIGNED32 api_version, UNSIGNED32 *max_version );
   typedef a_ads_connection_ptr  (WINAPI*ads_make_connection_func)( void * arg );
   typedef a_ads_connection_ptr  (WINAPI*ads_new_connection_func)( );
   typedef UNSIGNED32            (WINAPI*ads_num_cols_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*ads_num_params_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*ads_num_rows_func)( ADSHANDLE hStatement );
   typedef ADSHANDLE             (WINAPI*ads_prepare_func)( a_ads_connection *poConnect, UNSIGNED8 *pucSQL, UNSIGNED8 ucIsUnicode );
   typedef UNSIGNED32            (WINAPI*ads_reset_func)( ADSHANDLE hStatement );
   typedef UNSIGNED32            (WINAPI*ads_rollback_func)( a_ads_connection *poConnect );
   typedef UNSIGNED32            (WINAPI*ads_send_param_data_func)( ADSHANDLE hStatement, UNSIGNED32 ulIndex, UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength );
   typedef UNSIGNED32            (WINAPI*ads_sqlstate_func)( a_ads_connection *poConnect, UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength );

#if defined( __cplusplus )
}
#endif

/// @internal
#define function( x )   x ## _func x

typedef struct AdvantageInterface
   {
   /** DLL handle.
    */
   void * dll_handle;

   /** Flag to know if initialized or not.
    */
   int       initialized;

   /** Pointer to ::ads_init() function.
    */
   function( ads_init );

   /** Pointer to ::ads_fini() function.
    */
   function( ads_fini );

   /** Pointer to ::ads_new_connection() function.
    */
   function( ads_new_connection );

   /** Pointer to ::ads_free_connection() function.
    */
   function( ads_free_connection );

   /** Pointer to ::ads_make_connection() function.
    */
   function( ads_make_connection );

   /** Pointer to ::ads_connect() function.
    */
   function( ads_connect );

   /** Pointer to ::ads_disconnect() function.
    */
   function( ads_disconnect );

   /** Pointer to ::ads_execute_immediate() function.
    */
   function( ads_execute_immediate );

   /** Pointer to ::ads_prepare() function.
    */
   function( ads_prepare );

   /** Pointer to ::ads_free_stmt() function.
    */
   function( ads_free_stmt );

   /** Pointer to ::ads_num_params() function.
    */
   function( ads_num_params );

   /** Pointer to ::ads_describe_bind_param() function.
    */
   function( ads_describe_bind_param );

   /** Pointer to ::ads_bind_param() function.
    */
   function( ads_bind_param );

   /** Pointer to ::ads_send_param_data() function.
    */
   function( ads_send_param_data );

   /** Pointer to ::ads_reset() function.
    */
   function( ads_reset );

   /** Pointer to ::ads_get_bind_param_info() function.
    */
   function( ads_get_bind_param_info );

   /** Pointer to ::ads_execute() function.
    */
   function( ads_execute );

   /** Pointer to ::ads_execute_direct() function.
    */
   function( ads_execute_direct );

   /** Pointer to ::ads_fetch_absolute() function.
    */
   function( ads_fetch_absolute );

   /** Pointer to ::ads_fetch_next() function.
    */
   function( ads_fetch_next );

   /** Pointer to ::ads_get_next_result() function.
    */
   function( ads_get_next_result );

   /** Pointer to ::ads_affected_rows() function.
    */
   function( ads_affected_rows );

   /** Pointer to ::ads_num_cols() function.
    */
   function( ads_num_cols );

   /** Pointer to ::ads_num_rows() function.
    */
   function( ads_num_rows );

   /** Pointer to ::ads_get_column() function.
    */
   function( ads_get_column );

   /** Pointer to ::ads_get_data() function.
    */
   function( ads_get_data );

   /** Pointer to ::ads_get_data_info() function.
    */
   function( ads_get_data_info );

   /** Pointer to ::ads_get_column_info() function.
    */
   function( ads_get_column_info );

   /** Pointer to ::ads_commit() function.
    */
   function( ads_commit );

   /** Pointer to ::ads_rollback() function.
    */
   function( ads_rollback );

   /** Pointer to ::ads_client_version() function.
    */
   function( ads_client_version );

   /** Pointer to ::ads_error() function.
    */
   function( ads_error );

   /** Pointer to ::ads_sqlstate() function.
    */
   function( ads_sqlstate );

   /** Pointer to ::ads_clear_error() function.
    */
   function( ads_clear_error );

   /** Pointer to ::ads_cancel() function.
    */
   function( ads_cancel );

   /** Pointer to ::AdsBeginTransaction() function.
    */
   function( AdsBeginTransaction );

   } AdvantageInterface;
#undef function

#if defined( __cplusplus )
extern "C" {
#endif

   int ads_initialize_interface( AdvantageInterface * api, const char * optional_path_to_dll );

   void ads_finalize_interface( AdvantageInterface * api );

#if defined( __cplusplus )
}
#endif

#endif

