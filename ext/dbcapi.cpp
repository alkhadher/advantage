/*******************************************************************************
* Source File  : DBCAPI.CPP
* Copyright    : 2010 Sybase, an SAP company
* Description  : This contains all entry points to implement the DBCAPI interface
*                for many clients such as Python, PHP, Ruby, etc.
* Notes        :
*
* The purpose of this module is to implement a thin layer between our ACE client
* and Sybase's DBCAPI clients.  Bascially DBCAPI is a standard API shared between
* SQL Anywhere and Adaptive Server Enterprise for use with common thin clients.
* By implementing the same API in ACE we can consume those same thin clients
* and support many more development environments.
*******************************************************************************/

// This functionality is only used on the client side (in ACE)
#ifdef ACE

#include "adsinc.h"   
#include <ctype.h>
#include "ace.h"
#include "acetypes.h"
#include "aceclass.h"
#include "adsutil.h"
#include "cconnect.h"

#define ACE_ADSVERSION  // to get the ACE version string from adsver.h
#include "adsver.h"

#include "dbcapi.h"

#ifdef _DEBUG
   #define DBCLogTrace(x)
   //#define DBCLogTrace(x) printf( "%s\n", x )
#else
   #define DBCLogTrace(x)
#endif

#define DBCAPI_SUCCESS 1               // General success
#define DBCAPI_FAILURE 0               // General failure
#define DBCAPI_NEGONE (UNSIGNED32)-1   // Failure when returning a length


/*******************************************************************************
* Module       : ConvertACEtoNativeType
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : a_ads_native_type
* Desc         : Convert an ACE type to a native DBCAPI type
* Notes        : 
*******************************************************************************/
a_ads_native_type ConvertACEtoNativeType( UNSIGNED16 usACEType )
{
   switch ( usACEType )
      {
      case ADS_LOGICAL:
         return DT_BIT;
      case ADS_NUMERIC:
         return DT_DECIMAL;
      case ADS_DATE:
         return DT_DATE;
      case ADS_STRING:
         return DT_STRING;
      case ADS_MEMO:
         return DT_LONGVARCHAR;
      case ADS_BINARY:
         return DT_BINARY;
      case ADS_IMAGE:
         return DT_BINARY;
      case ADS_VARCHAR:
         return DT_VARCHAR;
      case ADS_COMPACTDATE:
         return DT_DATE;
      case ADS_DOUBLE:
         return DT_DOUBLE;
      case ADS_INTEGER:
         return DT_INT;
      case ADS_SHORTINT:
         return DT_SMALLINT;
      case ADS_TIME:
         return DT_TIME;
      case ADS_TIMESTAMP:
         return DT_TIMESTAMP;
      case ADS_AUTOINC:
         return DT_INT;
      case ADS_RAW:
         return DT_BINARY;
      case ADS_CURDOUBLE:
         return DT_DOUBLE;
      case ADS_MONEY:
         return DT_BIGINT;
      case ADS_LONGLONG:
         return DT_BIGINT;
      case ADS_CISTRING:
         return DT_STRING;
      case ADS_ROWVERSION:
         return DT_BIGINT;
      case ADS_MODTIME:
         return DT_TIMESTAMP;
      case ADS_VARCHAR_FOX:
         return DT_VARCHAR;
      case ADS_VARBINARY_FOX:
         return DT_BINARY;
      case ADS_SYSTEM_FIELD:
         return DT_NOTYPE;
      case ADS_NCHAR:
         return DT_NSTRING;
      case ADS_NVARCHAR:
         return DT_NSTRING;
      case ADS_NMEMO:
         return DT_LONGNVARCHAR;

      default:
         ASSERT( 0 );
         return DT_NOTYPE;
      }

} /* ConvertACEtoNativeType */


/*******************************************************************************
* Module       : ConvertACEToIOType
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : a_ads_data_type
* Desc         : Convert an ACE type to a DBCAPI IO type
* Notes        : 
*******************************************************************************/
a_ads_data_type ConvertACEToIOType( UNSIGNED16 usACEType )
{
   switch ( usACEType )
      {
      case ADS_LOGICAL:
         return A_UVAL8;
      case ADS_NUMERIC:
         return A_DECIMAL;
      case ADS_DATE:
         return A_DATE;
      case ADS_STRING:
         return A_STRING;
      case ADS_MEMO:
         return A_STRING;
      case ADS_BINARY:
         return A_BINARY;
      case ADS_IMAGE:
         return A_BINARY;
      case ADS_VARCHAR:
         return A_STRING;
      case ADS_COMPACTDATE:
         return A_DATE;
      case ADS_DOUBLE:
         return A_DOUBLE;
      case ADS_INTEGER:
         return A_VAL32;
      case ADS_SHORTINT:
         return A_VAL16;
      case ADS_TIME:
         return A_TIME;
      case ADS_TIMESTAMP:
         return A_TIMESTAMP;
      case ADS_AUTOINC:
         return A_UVAL32;
      case ADS_RAW:
         return A_BINARY;
      case ADS_CURDOUBLE:
         return A_DOUBLE;
      case ADS_MONEY:
         return A_VAL64;
      case ADS_LONGLONG:
         return A_VAL64;
      case ADS_CISTRING:
         return A_STRING;
      case ADS_ROWVERSION:
         return A_UVAL64;
      case ADS_MODTIME:
         return A_TIMESTAMP;
      case ADS_VARCHAR_FOX:
         return A_STRING;
      case ADS_VARBINARY_FOX:
         return A_BINARY;
      case ADS_SYSTEM_FIELD:
         return A_INVALID_TYPE;
      case ADS_NCHAR:
         return A_NCHAR;
      case ADS_NVARCHAR:
         return A_NCHAR;
      case ADS_NMEMO:
         return A_NCHAR;

      default:
         ASSERT( 0 );
         return A_INVALID_TYPE;
      }

} /*  ConvertACEToIOType */


/*******************************************************************************
* Module       : ads_affected_rows
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : Number of rows affected
* Desc         : Return the number of rows affected by a query
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_affected_rows( ADSHANDLE hStatement )
{
   UNSIGNED32 ulRows;
   UNSIGNED32 ulRetCode;

   DBCLogTrace( "ads_affected_rows" );

   // Returns rows affected for statement handles
   ulRetCode = AdsGetRecordCount( hStatement, ADS_IGNOREFILTERS, &ulRows );
   if ( ulRetCode == AE_SUCCESS )
      return ulRows;
   else
      return DBCAPI_FAILURE;

} /* ads_affected_rows */


/*******************************************************************************
* Module       : ads_bind_param
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : SUCCESS or FAILURE
* Desc         : Bind a user-supplied buffer as a parameter to the prepared statement.
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_bind_param( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param *param )
{
   UNSIGNED32 ulRetCode;

   DBCLogTrace( "ads_bind_param" );

   if ( param == NULL )
      return DBCAPI_FAILURE;

   if ( param->value.buffer == NULL )
      return DBCAPI_FAILURE;

   if ( param->value.length == NULL )
      return DBCAPI_FAILURE;

   // If the parameter value is NULL, set it to empty in ACE
   if ( param->value.is_null && *param->value.is_null )
      {
      if ( AdsSetEmpty( hStatement, ADSFIELD(ulIndex+1)) == AE_SUCCESS )
         return DBCAPI_SUCCESS;
      else
         return DBCAPI_FAILURE;
      }

   switch ( param->value.type )
      {
      case A_VAL32:
      case A_UVAL32:
         ulRetCode = AdsSetLong( hStatement, ADSFIELD(ulIndex+1), *((SIGNED32*)(param->value.buffer)) );
         break;

      case A_STRING:
         ulRetCode = AdsSetString( hStatement, ADSFIELD(ulIndex+1), param->value.buffer, *(param->value.length) );
         break;

      case A_BINARY:
         ulRetCode = AdsSetBinary( hStatement, ADSFIELD(ulIndex+1), ADS_BINARY, *(param->value.length),
                                   0, param->value.buffer, *(param->value.length) );
         break;

      case A_DOUBLE:
         ulRetCode = AdsSetDouble( hStatement, ADSFIELD(ulIndex+1), *((double*)(param->value.buffer)));
         break;

      case A_VAL64:
      case A_UVAL64: 
         ulRetCode = AdsSetLongLong( hStatement, ADSFIELD(ulIndex+1), *((SIGNED64*)(param->value.buffer)));
         break;

      case A_VAL16:
      case A_UVAL16:
         ulRetCode = AdsSetShort( hStatement, ADSFIELD(ulIndex+1), *((SIGNED16*)(param->value.buffer)));
         break;

      case A_VAL8:
      case A_UVAL8:
         ulRetCode = AdsSetLogical( hStatement, ADSFIELD(ulIndex+1), *((SIGNED8*)(param->value.buffer)));
         break;

      case A_NCHAR:
         {
         // Unicode buffer lengths are always in bytes, but AdsSetStringW needs it in chars (UTF-16)
         UNSIGNED32 ulChars = (*(param->value.length) >> 1) - 2; // -2 for the BOM
         ulRetCode = AdsSetStringW( hStatement, ADSFIELD(ulIndex+1), 
                                    (WCHAR*)(param->value.buffer) + 1, // skip the BOM
                                    ulChars );
         }
         break;

      default:
         ulRetCode = AdsSetLastError( AE_INVALID_DATA_TYPE, "Unsupported parameter data type." );
         break;
      }

   if ( ulRetCode == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_bind_param */


/*******************************************************************************
* Module       : DBCAPICallback
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 1 to cancel the operation, 0 to keep it going
* Desc         : Callback funtion to support query cancel
* Notes        : This is a 64 bit FN and must be registered with AdsRegisterCallbackFunction101
*******************************************************************************/
UNSIGNED32 WINAPI DBCAPICallback( UNSIGNED16 usPercentDone, SIGNED64 qCallbackID )
{
   UNSIGNED32 ulCancel = 0;

#ifdef x64
   if ( AdsSetProperty90( (ADSHANDLE)qCallbackID, ADS_DBCAPI_CANCEL, (UNSIGNED64)&ulCancel ) == AE_SUCCESS )
#else
   if ( AdsSetProperty( (ADSHANDLE)qCallbackID, ADS_DBCAPI_CANCEL, (UNSIGNED32)&ulCancel ) == AE_SUCCESS )
#endif
      return ulCancel;

   return 0;

}  /* ShowPercentage */


/*******************************************************************************
* Module       : ads_cancel
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value :
* Desc         : Cancel a query
* Notes        :
*******************************************************************************/
void ENTRYPOINT ads_cancel( a_ads_connection *poConnect )
{
   UNSIGNED32 ulCancel = TRUE;

   DBCLogTrace( "ads_cancel" );

   // Set the cancel flag on the connection and the next time the callback
   // function gets called with this connection, it will abort the query.
   // Not using AdsSetProperty here since we already have the connection sync and
   // calling an API would block.
   if ( poConnect && poConnect->hConnect && ValidConnectionHandle( poConnect->hConnect ))
      ((CCONNECTION*)poConnect->hConnect)->GetDBCAPICancel( &ulCancel );

} /* ads_cancel */


/*******************************************************************************
* Module       : ads_clear_error
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Clear the current/last error
* Notes        : 
*******************************************************************************/
void ENTRYPOINT ads_clear_error( a_ads_connection *poConnect )
{
   DBCLogTrace( "ads_clear_error" );
   AdsClearLastError();

} /* ads_clear_error */


/*******************************************************************************
* Module       : ads_client_version
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Return the client version
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_client_version( UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength )
{
   DBCLogTrace( "ads_client_version" );
   if ( pucBuffer && ulLength )
      {
      AXSTRNCPY( (SIGNED8*)pucBuffer, pcIdAxsVer + 9, ulLength );
      *(pucBuffer + ulLength - 1) = 0x00;  // ensure NULL terminator
      return DBCAPI_SUCCESS;
      }

   return DBCAPI_FAILURE;

} /* ads_client_version */


/*******************************************************************************
* Module       : ads_commit
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Commit the current transaction
* Notes        :
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_commit( a_ads_connection *poConnect )
{
   DBCLogTrace( "ads_commit" );
   
   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   if ( AdsCommitTransaction( poConnect->hConnect ) == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_commit */


/*******************************************************************************
* Module       : ads_connect
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Connect to Advantage
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_connect( a_ads_connection *poConnect, UNSIGNED8 *pucConnectString )
{
   DBCLogTrace( "ads_connect" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   if ( AdsConnect101( pucConnectString, NULL, &(poConnect->hConnect) ) == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_connect */


/*******************************************************************************
* Module       : ads_describe_bind_param
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Describe a parameter
* Notes        : Not fully functional...
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_describe_bind_param( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param *param )
{
   DBCLogTrace( "ads_describe_bind_param" );

   // Since ACE doesn't know anything about the type of parameters in a prepared
   // statement, we can't tell the type of a parameter.  All we know is that
   // it must be an INPUT parameter since we don't support OUTPUT (or INPUT_OUTPUT) ones.
   // If in the future ACE becomes aware of parameters (like our ODBC client), we can update
   // this function to do what it is supposed to.  The effect of this is that the caller must
   // set the data type themselves.
   if ( hStatement == 0 )
      return DBCAPI_FAILURE;

   if ( param == NULL )
      return DBCAPI_FAILURE;

   // All parameters must be INPUT for ADS
   param->direction = DD_INPUT;

   return DBCAPI_SUCCESS;

} /* ads_describe_bind_param */


/*******************************************************************************
* Module       : ads_disconnect
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Disconnect from Advantage
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_disconnect( a_ads_connection *poConnect )
{
   UNSIGNED32 ulRetCode;

   DBCLogTrace( "ads_disconnect" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   ulRetCode = AdsDisconnect( poConnect->hConnect );
   poConnect->hConnect = 0;

   if ( ulRetCode == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_disconnect */


/*******************************************************************************
* Module       : ads_error
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Get the last (current) error
* Notes        :
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_error( a_ads_connection *poConnect, UNSIGNED8 *pucError, UNSIGNED32 ulLength )
{
   UNSIGNED32 ulError;
   UNSIGNED16 usLen = (UNSIGNED16)ulLength;

   DBCLogTrace( "ads_error" );

   AdsGetLastError( &ulError, pucError, &usLen );
   if ( pucError && ulLength )
      *(pucError + ulLength - 1) = 0x00;

   return ulError;

} /* ads_error */


/*******************************************************************************
* Module       : SetupCursorBuffer
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Allocate a record buffer structure to hold values as they are fetched
* Notes        : 
*******************************************************************************/
UNSIGNED32 SetupCursorBuffer( ADSHANDLE hCursor )
{
   UNSIGNED32    ulAllocationSize;
   UNSIGNED8     *pucFieldData;
   DBCAPI_COLUMN *pstFieldStruct;
   UNSIGNED16    usIndex;
   UNSIGNED16    usFieldCount;
   UNSIGNED16    usFieldType;
   UNSIGNED32    ulFieldLength;
   UNSIGNED32    ulRetCode;
   UNSIGNED16    usLen;
   UNSIGNED8     aucFieldName[ADS_MAX_FIELD_NAME + 1];

   // If we're re-executing a query that was already open we might be reusing the
   // ACE cursor object.  In that case, we will already have the DBCAPI_COLUMN structures
   // initialized.
#ifdef x64
   ulRetCode = AdsSetProperty90( hCursor, ADS_GET_DBCAPI_BUFFER, (UNSIGNED64)&pstFieldStruct );
#else
   ulRetCode = AdsSetProperty( hCursor, ADS_GET_DBCAPI_BUFFER, (UNSIGNED32)&pstFieldStruct );
#endif

   if (( ulRetCode == AE_SUCCESS ) && pstFieldStruct )
      // That was easy!
      return AE_SUCCESS;

   ulRetCode = AdsGetNumFields( hCursor, &usFieldCount );
   if ( ulRetCode != AE_SUCCESS )
      return ulRetCode;

   ulAllocationSize = usFieldCount * sizeof( DBCAPI_COLUMN );

   // Calculate how much extra memory we need for each column's data
   for ( usIndex = 0; usIndex < usFieldCount; usIndex++ )
      {
      // Include some memory for the field name
      usLen = sizeof( aucFieldName );
      ulRetCode = AdsGetFieldName( hCursor, usIndex + 1, aucFieldName, &usLen );
      if ( ulRetCode != AE_SUCCESS )
         return ulRetCode;

      ulAllocationSize += usLen + 1;

      ulRetCode = AdsGetFieldType( hCursor, ADSFIELD( usIndex + 1), &usFieldType );
      if ( ulRetCode != AE_SUCCESS )
         return ulRetCode;

      switch ( usFieldType )
         {
         case ADS_MEMO:
         case ADS_NMEMO:
         case ADS_IMAGE:
         case ADS_BINARY:
         case ADS_VARCHAR:
            // Blob fields use a dynamic buffer
            break;

         case ADS_TIME:
            // Time gets converted to strings
            ulAllocationSize += 39;  // max length if values are wacky
            break;

         case ADS_DATE:
         case ADS_COMPACTDATE:
            // Dates get converted to strings
            ulAllocationSize += ADS_MAX_DATEMASK;
            break;

         case ADS_TIMESTAMP:
         case ADS_MODTIME:
            // Timestamps get converted to strings
            ulAllocationSize += 39 + ADS_MAX_DATEMASK;
            break;

         case ADS_NCHAR:
         case ADS_NVARCHAR:
            // Unicode char fields
            ulRetCode = AdsGetFieldLength100( hCursor, ADSFIELD( usIndex + 1 ), ADS_BYTE_LENGTH, &ulFieldLength );
            if ( ulRetCode != AE_SUCCESS )
               return ulRetCode;

            ulAllocationSize += ulFieldLength + 2;  // +2 for the double NULL
            break;

         case ADS_STRING:
         case ADS_CISTRING:
         case ADS_VARCHAR_FOX:
         case ADS_NUMERIC:
            // Strings need one extra byte for the NULL terminator
            ulAllocationSize += 1;

         default:
            // Everything else needs the field length amount of bytes
            ulRetCode = AdsGetFieldLength100( hCursor, ADSFIELD( usIndex + 1 ), ADS_BYTE_LENGTH, &ulFieldLength );
            if ( ulRetCode != AE_SUCCESS )
               return ulRetCode;

            ulAllocationSize += ulFieldLength;
            break;
         }
      }

   pstFieldStruct = (DBCAPI_COLUMN*)MAllocateMemory( ulAllocationSize );
   if ( pstFieldStruct == NULL )
      return AdsSetLastError( AE_ALLOCATION_FAILED, NULL );

   pucFieldData = (UNSIGNED8*)pstFieldStruct + (usFieldCount * sizeof( DBCAPI_COLUMN ));

   for ( usIndex = 0; usIndex < usFieldCount; usIndex++ )
      {
      usLen = sizeof( aucFieldName );
      ulRetCode = AdsGetFieldName( hCursor, usIndex + 1, pucFieldData, &usLen );
      if ( ulRetCode != AE_SUCCESS )
         return ulRetCode;

      pstFieldStruct[usIndex].pucColumnName = pucFieldData;
      pucFieldData += usLen + 1;

      ulRetCode = AdsGetFieldType( hCursor, ADSFIELD( usIndex + 1), &usFieldType );
      if ( ulRetCode != AE_SUCCESS )
         return ulRetCode;

      pstFieldStruct[usIndex].ulIsNull = 0;
      switch ( usFieldType )
         {
         case ADS_MEMO:
         case ADS_NMEMO:
         case ADS_IMAGE:
         case ADS_BINARY:
         case ADS_VARCHAR:
            // Blob fields use dynamic memory
            pstFieldStruct[usIndex].pucData = NULL;
            pstFieldStruct[usIndex].ulBufferLength = 0;
            break;

         case ADS_TIME:
            // Time gets converted to strings
            pstFieldStruct[usIndex].pucData = pucFieldData;
            pstFieldStruct[usIndex].ulBufferLength = 39;
            pucFieldData += 39;
            break;

         case ADS_DATE:
         case ADS_COMPACTDATE:
            // Dates get converted to strings
            pstFieldStruct[usIndex].pucData = pucFieldData;
            pstFieldStruct[usIndex].ulBufferLength = ADS_MAX_DATEMASK;
            pucFieldData += ADS_MAX_DATEMASK;
            break;

         case ADS_TIMESTAMP:
         case ADS_MODTIME:
            // Timestamps get converted to strings
            pstFieldStruct[usIndex].pucData = pucFieldData;
            pstFieldStruct[usIndex].ulBufferLength = 39 + ADS_MAX_DATEMASK;
            pucFieldData += 39 + ADS_MAX_DATEMASK;
            break;

         case ADS_STRING:
         case ADS_CISTRING:
         case ADS_VARCHAR_FOX:
         case ADS_NUMERIC:
            ulRetCode = AdsGetFieldLength100( hCursor, ADSFIELD( usIndex + 1 ), ADS_BYTE_LENGTH, &ulFieldLength );
            if ( ulRetCode != AE_SUCCESS )
               return ulRetCode;

            pstFieldStruct[usIndex].pucData = pucFieldData;
            pstFieldStruct[usIndex].ulBufferLength = ulFieldLength + 1;
            pucFieldData += ulFieldLength + 1;
            break;

         case ADS_NCHAR:
         case ADS_NVARCHAR:
            ulRetCode = AdsGetFieldLength100( hCursor, ADSFIELD( usIndex + 1 ), ADS_BYTE_LENGTH, &ulFieldLength );
            if ( ulRetCode != AE_SUCCESS )
               return ulRetCode;

            pstFieldStruct[usIndex].pucData = pucFieldData;
            pstFieldStruct[usIndex].ulBufferLength = ulFieldLength + 2;
            pucFieldData += ulFieldLength + 1;
            break;

         default:
            ulRetCode = AdsGetFieldLength100( hCursor, ADSFIELD( usIndex + 1 ), ADS_BYTE_LENGTH, &ulFieldLength );
            if ( ulRetCode != AE_SUCCESS )
               return ulRetCode;

            pstFieldStruct[usIndex].pucData = pucFieldData;
            pstFieldStruct[usIndex].ulBufferLength = ulFieldLength;
            pucFieldData += ulFieldLength;
            break;
         }
      }

   // Store the field structure pointer in the cursor object
   return AdsSetProperty90( hCursor, ADS_SET_DBCAPI_BUFFER, (UNSIGNED64)pstFieldStruct );

} /* SetupCursorBuffer */


/*******************************************************************************
* Module       : ads_execute
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Execute a prepared query
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_execute( ADSHANDLE hStatement )
{
   ADSHANDLE  hCursor;
   UNSIGNED32 ulRetCode;
   ADSHANDLE  hConnect;

   DBCLogTrace( "ads_execute" );

   ulRetCode = AdsGetConnectionHandle( hStatement, &hConnect );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   ulRetCode = AdsRegisterCallbackFunction101( DBCAPICallback, hConnect );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   ulRetCode = AdsExecuteSQL( hStatement, &hCursor );
   if (( ulRetCode == AE_SUCCESS ) && hCursor )
      {
      ulRetCode = SetupCursorBuffer( hCursor );
      if ( ulRetCode != AE_SUCCESS )
         AdsCloseTable( hCursor );
      }

   if ( ulRetCode == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_execute */


/*******************************************************************************
* Module       : ads_execute_direct
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Execute a query
* Notes        : 
*******************************************************************************/
ADSHANDLE ENTRYPOINT ads_execute_direct( a_ads_connection *poConnect, UNSIGNED8 *pucSQL )
{
   UNSIGNED32 ulRetCode;
   ADSHANDLE  hCursor;
   ADSHANDLE  hStatement;

   DBCLogTrace( "ads_execute_direct" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   ulRetCode = AdsCreateSQLStatement( poConnect->hConnect, &hStatement );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   ulRetCode = AdsRegisterCallbackFunction101( DBCAPICallback, poConnect->hConnect );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   ulRetCode = AdsExecuteSQLDirect( hStatement, pucSQL, &hCursor );
   if ( ulRetCode != AE_SUCCESS )
      {
      AdsSetProperty( poConnect->hConnect, ADS_PUSH_ERROR_STACK, 0 );
      AdsCloseSQLStatement( hStatement );
      AdsSetProperty( poConnect->hConnect, ADS_POP_ERROR_STACK, 0 );
      return DBCAPI_FAILURE;
      }
   else if ( hCursor )
      {
      ulRetCode = SetupCursorBuffer( hCursor );
      if ( ulRetCode != AE_SUCCESS )
         {
         AdsCloseTable( hCursor );
         AdsCloseSQLStatement( hStatement );
         return DBCAPI_FAILURE;
         }
      }

   return hStatement;

} /* ads_execute_direct */


/*******************************************************************************
* Module       : ads_execute_immediate
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Execute a query that has no results
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_execute_immediate( a_ads_connection *poConnect, UNSIGNED8 *pucSQL )
{
   ADSHANDLE  hCursor = 0;
   UNSIGNED32 ulRetCode;
   ADSHANDLE  hStatement;

   DBCLogTrace( "ads_execute_immediate" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   ulRetCode = AdsCreateSQLStatement( poConnect->hConnect, &hStatement );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;
   
   ulRetCode = AdsRegisterCallbackFunction101( DBCAPICallback, poConnect->hConnect );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   ulRetCode = AdsExecuteSQLDirect( hStatement, pucSQL, &hCursor );
   if (( ulRetCode == AE_SUCCESS ) && hCursor )
      // There shouldn't be a result set with this API
      AdsCloseTable( hCursor );

   if ( ulRetCode == AE_SUCCESS )
      AdsCloseSQLStatement( hStatement );
   else
      {
      AdsSetProperty( poConnect->hConnect, ADS_PUSH_ERROR_STACK, 0 );
      AdsCloseSQLStatement( hStatement );
      AdsSetProperty( poConnect->hConnect, ADS_POP_ERROR_STACK, 0 );
      }

   if ( ulRetCode == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_execute_immediate */


/*******************************************************************************
* Module       : ads_fetch_absolute
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Fetch a specific record in a cursor
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_fetch_absolute( ADSHANDLE hStatement, UNSIGNED32 ulRowNum )
{
   ADSHANDLE hCursor;

   DBCLogTrace( "ads_fetch_absolute" );

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( AdsGotoRecord( hCursor, ulRowNum ) != AE_SUCCESS )
      return DBCAPI_FAILURE;
   else
      return DBCAPI_SUCCESS;

} /* ads_fetch_absolute */


/*******************************************************************************
* Module       : ads_fetch_next
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Fetch the next record, ie Skip to the next record
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_fetch_next( ADSHANDLE hStatement )
{
   ADSHANDLE  hCursor;

   DBCLogTrace( "ads_fetch_next" );

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return FALSE;

   if ( hCursor )
      {
      UNSIGNED32 ulFirstFetch;
      UNSIGNED32 ulRetCode;
      UNSIGNED16 usIsDeleted;

      // Peek into the cursor object and see if this is the first time we fetch a record
#ifdef x64
      if ( AdsSetProperty90( hCursor, ADS_IS_FIRST_FETCH, (UNSIGNED64)&ulFirstFetch ) != AE_SUCCESS )
#else
      if ( AdsSetProperty( hCursor, ADS_IS_FIRST_FETCH, (UNSIGNED32)&ulFirstFetch ) != AE_SUCCESS )
#endif
         return FALSE;

      // Since ACE automatically puts us at the first record, don't skip for the first fetch
      if ( ulFirstFetch == FALSE )
         {
         if ( AdsSkip( hCursor, 1 ) != AE_SUCCESS )
            return FALSE;
         }

      // If we are not positioned, this API will return AE_NO_CURRENT_RECORD
      ulRetCode = AdsIsRecordDeleted( hCursor, &usIsDeleted );
      if ( ulRetCode != AE_SUCCESS )
         {
         if ( ulRetCode == AE_NO_CURRENT_RECORD )
            // Not really an error
            AdsClearLastError();

         return FALSE;
         }

      // Don't really care about usIsDeleted, as long as we are positioned
      return TRUE;
      }
   else
      return FALSE;

} /* ads_fetch_next */


/*******************************************************************************
* Module       : ads_fini
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Perform any finalization tasks
* Notes        :
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_fini( void )
{
   DBCLogTrace( "ads_fini" );
   
   AdsDisconnect( 0 );

   return DBCAPI_SUCCESS;

} /* ads_fini */


/*******************************************************************************
* Module       : ads_free_connection
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Free an a_ads_connection object & disconnect if necessary
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_free_connection( a_ads_connection *poConnect )
{
   DBCLogTrace( "ads_free_connection" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   if ( poConnect->hConnect )
      AdsDisconnect( poConnect->hConnect );

   ReleaseMemory( poConnect );

   return DBCAPI_SUCCESS;

} /* ads_free_connection */


/*******************************************************************************
* Module       : ads_free_stmt
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Free a statement
* Notes        : This will free any open cursors & the DBCAPI_COLUMN data within
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_free_stmt( ADSHANDLE hStatement )
{
   DBCLogTrace( "ads_free_stmt" );

   if ( hStatement == 0 )
      return DBCAPI_FAILURE;

   if ( AdsCloseSQLStatement( hStatement ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   return DBCAPI_SUCCESS;

} /* ads_free_stmt */


/*******************************************************************************
* Module       : ads_get_bind_param_info
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Retrieves information about the parameters that were bound using ads_bind_param
* Notes        : Currently not supported
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_get_bind_param_info( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_bind_param_info *info )
{
   DBCLogTrace( "ads_get_bind_param_info" );

   // UNRESOLVED PF: I think we could support this if the parameter has 
   // already been set.  The statement in ACE has the parameter name, number,
   // type, and length.  Since we don't support binding variables to parameters
   // this function will be incomplete and perhaps not very useful anyway.
   return DBCAPI_FAILURE;

} /* ads_get_bind_param_info */


/*******************************************************************************
* Module       : DBCAPIGetData
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Utility FN to get column data
* Notes        : 
*******************************************************************************/
UNSIGNED32 DBCAPIGetData
(
   ADSHANDLE  hCursor,
   a_ads_data_type usDataType,
   UNSIGNED32 ulIndex,
   UNSIGNED8  **ppucBuffer,
   UNSIGNED32 *pulLength,
   UNSIGNED32 *pulBufferLength,
   UNSIGNED16 *pusIsNull,
   UNSIGNED32 ulOffset
)
{
   UNSIGNED32 ulRetCode;
   UNSIGNED16 usFieldType;

   ulRetCode = AdsIsNull( hCursor, ADSFIELD(ulIndex+1), pusIsNull );
   if ( ulRetCode != AE_SUCCESS )
      return ulRetCode;

   switch ( usDataType )
      {
      case A_VAL16:
         ulRetCode = AdsGetShort( hCursor, ADSFIELD(ulIndex+1), (SIGNED16*)*ppucBuffer );
         if ( ulRetCode != AE_SUCCESS )
            return ulRetCode;

         *pulLength = sizeof( SIGNED16 );
         break;

      case A_UVAL8:
         {
         UNSIGNED16 usShort;
         ulRetCode = AdsGetLogical( hCursor, ADSFIELD(ulIndex+1), &usShort );
         if ( ulRetCode != AE_SUCCESS )
            return ulRetCode;

         **ppucBuffer = (UNSIGNED8)usShort;
         *pulLength = sizeof( UNSIGNED8 );
         }
         break;

      case A_VAL32:
      case A_UVAL32:
         ulRetCode = AdsGetLong( hCursor, ADSFIELD(ulIndex+1), (SIGNED32*)*ppucBuffer );
         if ( ulRetCode != AE_SUCCESS )
            return ulRetCode;

         *pulLength = sizeof( SIGNED32 );
         break;

      case A_DOUBLE:
         ulRetCode = AdsGetDouble( hCursor, ADSFIELD(ulIndex+1), (double*)*ppucBuffer );
         if ( ulRetCode != AE_SUCCESS )
            return ulRetCode;

         *pulLength = sizeof( double );
         break;

      case A_VAL64:
      case A_UVAL64:
         ulRetCode = AdsGetLongLong( hCursor, ADSFIELD(ulIndex+1), (SIGNED64*)*ppucBuffer );
         if ( ulRetCode != AE_SUCCESS )
            return ulRetCode;

         *pulLength = sizeof( SIGNED64 );
         break;

      default:
         // If no buffer length is provided, the user provided us the buffer so just call AdsGetField
         if ( !pulBufferLength )
            {
            if ( ulOffset )
               return AdsGetBinary( hCursor, ADSFIELD(ulIndex+1), ulOffset, *ppucBuffer, pulLength );
            else
               return AdsGetField( hCursor, ADSFIELD(ulIndex+1), *ppucBuffer, pulLength, ADS_NONE );
            }

         ulRetCode = AdsGetFieldType( hCursor, ADSFIELD( ulIndex+1 ), &usFieldType );
         if ( ulRetCode != AE_SUCCESS )
            return ulRetCode;

         switch ( usFieldType )
            {
            case ADS_MEMO:
            case ADS_NMEMO:
            case ADS_IMAGE:
            case ADS_BINARY:
            case ADS_VARCHAR:
               // BLOB fields use a dynamic buffer to store the data
               if ( *pusIsNull )
                  *pulLength = 0;
               else
                  {
                  UNSIGNED32 ulBlobLength;

                  ulRetCode = AdsGetMemoLength( hCursor, ADSFIELD(ulIndex+1), &ulBlobLength );
                  if ( ulRetCode != AE_SUCCESS )
                     return ulRetCode;

                  // Memo (string) fields need an extra byte for the NULL
                  if (( usFieldType == ADS_MEMO ) || ( usFieldType == ADS_VARCHAR ))
                     ulBlobLength++;

                  else if ( usFieldType == ADS_NMEMO )
                     ulBlobLength = ( ulBlobLength + 1 ) * 2;

                  // Allocate a larger buffer if necessary
                  if ( *pulBufferLength < ulBlobLength )
                     {
                     if ( *ppucBuffer )
                        *ppucBuffer = (UNSIGNED8*)ReallocateMemory( *ppucBuffer, ulBlobLength );
                     else
                        *ppucBuffer = (UNSIGNED8*)MAllocateMemory( ulBlobLength );

                     if ( *ppucBuffer == NULL )
                        return AdsSetLastError( AE_ALLOCATION_FAILED, NULL );

                     *pulBufferLength = ulBlobLength;
                     }

                  *pulLength = *pulBufferLength;
                  if (( usFieldType == ADS_MEMO ) || ( usFieldType == ADS_VARCHAR ))
                     ulRetCode = AdsGetString( hCursor, ADSFIELD(ulIndex+1), *ppucBuffer, pulLength, ADS_NONE );

                  else if ( usFieldType == ADS_NMEMO )
                     {
                     // AdsGetStringW expects number of chars, not bytes
                     *pulLength >>= 1;
                     ulRetCode = AdsGetStringW( hCursor, ADSFIELD(ulIndex+1), (WCHAR*)*ppucBuffer, pulLength, ADS_NONE );
                     if ( ulRetCode == AE_SUCCESS )
                        *pulLength <<= 1;
                     }

                  else
                     ulRetCode = AdsGetBinary( hCursor, ADSFIELD(ulIndex+1), 0, *ppucBuffer, pulLength );

                  if ( ulRetCode != AE_SUCCESS )
                     return ulRetCode;
                  }
               break;

            case ADS_NCHAR:
            case ADS_NVARCHAR:
               *pulLength = *pulBufferLength;
               *pulLength >>= 1;
               ulRetCode = AdsGetStringW( hCursor, ADSFIELD(ulIndex+1), (WCHAR*)*ppucBuffer, pulLength, ADS_NONE );
               if ( ulRetCode == AE_SUCCESS )
                  *pulLength <<= 1;
               break;

            default:
               // Non blob fields can just use AdsGetField
               *pulLength = *pulBufferLength;
               ulRetCode = AdsGetField( hCursor, ADSFIELD(ulIndex+1), *ppucBuffer, pulLength, ADS_NONE );
               if ( ulRetCode != AE_SUCCESS )
                  return ulRetCode;

               break;

            }  // switch ( ACE type )

         break;

      }  //  switch( DBCAPI type )

   return ulRetCode;

} /* DBCAPIGetData */


/*******************************************************************************
* Module       : ads_get_column
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Get the value of a column
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_get_column( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_data_value *buffer )
{
   ADSHANDLE      hCursor;
   UNSIGNED16     usIsNull;
   UNSIGNED32     ulRetCode;
   DBCAPI_COLUMN  *pstColumnStruct;
   UNSIGNED16     usFieldType;

   DBCLogTrace( "ads_get_column" );

   if ( buffer == NULL )
      return DBCAPI_FAILURE;

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

#ifdef x64
   ulRetCode = AdsSetProperty90( hCursor, ADS_GET_DBCAPI_BUFFER, (UNSIGNED64)&pstColumnStruct );
#else
   ulRetCode = AdsSetProperty( hCursor, ADS_GET_DBCAPI_BUFFER, (UNSIGNED32)&pstColumnStruct );
#endif

   if (( ulRetCode != AE_SUCCESS ) || !pstColumnStruct )
      return DBCAPI_FAILURE;

   pstColumnStruct += ulIndex;

   if ( AdsGetFieldType( hCursor, ADSFIELD( ulIndex + 1 ), &usFieldType ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   buffer->type = ConvertACEToIOType( usFieldType );
   ulRetCode = DBCAPIGetData( hCursor, buffer->type, ulIndex,
                              &(pstColumnStruct->pucData),
                              &(pstColumnStruct->ulDataLength),
                              &(pstColumnStruct->ulBufferLength),
                              &usIsNull, 0 );
   if ( ulRetCode == AE_SUCCESS )
      {
      buffer->buffer = pstColumnStruct->pucData;
      buffer->length = &(pstColumnStruct->ulDataLength);
      buffer->is_null = &(pstColumnStruct->ulIsNull);
      buffer->buffer_size = pstColumnStruct->ulBufferLength;
      *(buffer->is_null) = usIsNull;
      return DBCAPI_SUCCESS;
      }
   else
      {
      *(buffer->length) = 0;
      buffer->buffer_size = 0;
      buffer->buffer = NULL;
      buffer->type = A_INVALID_TYPE;
      *(buffer->is_null) = 0;
      return DBCAPI_NEGONE;
      }

} /* ads_get_column */


/*******************************************************************************
* Module       : ads_get_column_info
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Get some info about a column
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_get_column_info( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_column_info *buffer )
{
   ADSHANDLE      hCursor;
   UNSIGNED16     usFieldType;
   UNSIGNED32     ulFieldLength;
   UNSIGNED16     usFieldDecimals;
   UNSIGNED16     usIsNullable;
   DBCAPI_COLUMN  *pstColumnStruct;
   UNSIGNED32     ulRetCode;

   DBCLogTrace( "ads_get_column_info" );

   if ( buffer == NULL )
      return DBCAPI_FAILURE;

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( AdsGetFieldType( hCursor, ADSFIELD( ulIndex + 1 ), &usFieldType ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( AdsGetFieldLength100( hCursor, ADSFIELD( ulIndex + 1 ), ADS_BYTE_LENGTH, &ulFieldLength ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( AdsGetFieldDecimals( hCursor, ADSFIELD( ulIndex + 1 ), &usFieldDecimals ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( AdsIsNullable( hCursor, ADSFIELD( ulIndex + 1 ), &usIsNullable ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

#ifdef x64
   ulRetCode = AdsSetProperty90( hCursor, ADS_GET_DBCAPI_BUFFER, (UNSIGNED64)&pstColumnStruct );
#else
   ulRetCode = AdsSetProperty( hCursor, ADS_GET_DBCAPI_BUFFER, (UNSIGNED32)&pstColumnStruct );
#endif

   if (( ulRetCode != AE_SUCCESS ) || !pstColumnStruct )
      return DBCAPI_FAILURE;

   pstColumnStruct += ulIndex;

   buffer->name = pstColumnStruct->pucColumnName;
   buffer->max_size = ulFieldLength;
   buffer->precision = (UNSIGNED16)ulFieldLength;
   buffer->scale = usFieldDecimals;
   buffer->nullable = usIsNullable;
   buffer->type = ConvertACEToIOType( usFieldType );
   buffer->native_type = ConvertACEtoNativeType( usFieldType );

   return DBCAPI_SUCCESS;


} /* ads_get_column_info */


/*******************************************************************************
* Module       : ads_get_data
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Get data from a column
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_get_data( ADSHANDLE hStatement, UNSIGNED32 ulIndex, UNSIGNED32 ulOffset, void *buffer, UNSIGNED32 ulLength )
{
   UNSIGNED32 ulRetCode;
   UNSIGNED16 usIsNull;
   UNSIGNED16 usFieldType;
   ADSHANDLE  hCursor;

   DBCLogTrace( "ads_get_data" );

   if ( buffer == NULL )
      return DBCAPI_NEGONE;

   if ( hStatement == 0 )
      return DBCAPI_NEGONE;

   ulRetCode = AdsStmtGetCursorHandle( hStatement, &hCursor );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_NEGONE;

   ulRetCode = AdsGetFieldType( hCursor, ADSFIELD(ulIndex+1), &usFieldType );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_NEGONE;

   ulRetCode = DBCAPIGetData( hCursor, ConvertACEToIOType( usFieldType ),
                              ulIndex, (UNSIGNED8**)&buffer,
                              &ulLength, NULL, &usIsNull, ulOffset );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_NEGONE;

   return ulLength;

} /* ads_get_data */


/*******************************************************************************
* Module       : ads_get_data_info
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Get some info about a column's value
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_get_data_info( ADSHANDLE hStatement, UNSIGNED32 ulIndex, a_ads_data_info *buffer )
{
   UNSIGNED32 ulRetCode;
   UNSIGNED16 usType;
   UNSIGNED16 usIsNull;
   UNSIGNED32 ulDataLength;
   ADSHANDLE  hCursor;

   DBCLogTrace( "ads_get_data_info" );

   if ( buffer == NULL )
      return DBCAPI_FAILURE;

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( hCursor == 0 )
      return DBCAPI_FAILURE;

   ulRetCode = AdsGetFieldType( hCursor, ADSFIELD(ulIndex+1), &usType );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   ulRetCode = AdsIsNull( hCursor, ADSFIELD(ulIndex+1), &usIsNull );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if (( usType == ADS_MEMO ) ||
       ( usType == ADS_NMEMO ) ||
       ( usType == ADS_VARCHAR ) ||
       ( usType == ADS_BINARY ) ||
       ( usType == ADS_IMAGE ))
      ulRetCode = AdsGetMemoLength( hCursor, ADSFIELD(ulIndex+1), &ulDataLength );
   else
      ulRetCode = AdsGetFieldLength100( hCursor, ADSFIELD(ulIndex+1), ADS_BYTE_LENGTH, &ulDataLength );

   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   buffer->type = ConvertACEToIOType( usType );
   buffer->is_null = usIsNull;
   buffer->data_size = ulDataLength;

   return DBCAPI_SUCCESS;

} /* ads_get_data_info */


/*******************************************************************************
* Module       : ads_get_next_result
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Get the next result set from the statement
* Notes        : Unsupported with ADS
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_get_next_result( ADSHANDLE hStatement )
{
   DBCLogTrace( "ads_get_next_result" );

   // ADS doesn't support multiple result sets on a single statement
   return DBCAPI_FAILURE;

} /* ads_get_next_result */


/*******************************************************************************
* Module       : ads_init
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Initialize the DBCAPI client
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_init( UNSIGNED8 *app_name, UNSIGNED32 api_version, UNSIGNED32 *version_available )
{
   DBCLogTrace( "ads_init" );

   // We support the more basic version of DBCAPI, version 1
   *version_available = 1;

   return DBCAPI_SUCCESS;

} /* ads_init */


/*******************************************************************************
* Module       : ads_make_connection
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : "make" a connection from a DBLIB SQLCA object
* Notes        :
*******************************************************************************/
a_ads_connection_ptr ENTRYPOINT ads_make_connection( void *arg )
{
   DBCLogTrace( "ads_make_connection" );

   // Create a connection object based on a supplied DBLIB SQLCA pointer
   // UNRESOLVED how to interpret a DBLIB SQLCA object?
   return NULL;

} /* ads_make_connection */


/*******************************************************************************
* Module       : ads_new_connection
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Allocate a new a_ads_connection object
* Notes        : Doesn't connect, just creates a new object
*******************************************************************************/
a_ads_connection_ptr ENTRYPOINT ads_new_connection( void )
{
   a_ads_connection *poConnect;

   DBCLogTrace( "ads_new_connection" );

   // Return a DBCAPI connection object, later we'll get a real connection via ads_connect
   poConnect = (a_ads_connection*)MAllocateMemory( sizeof( a_ads_connection ));
   if ( poConnect )
      memset( poConnect, 0, sizeof( a_ads_connection ));

   return poConnect;

} /* ads_new_connection */


/*******************************************************************************
* Module       : ads_num_cols
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Return the number of columns in the cursor
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_num_cols( ADSHANDLE hStatement )
{
   ADSHANDLE  hCursor;
   UNSIGNED16 usFieldCount;

   DBCLogTrace( "ads_num_cols" );

   if ( hStatement == 0 )
      return DBCAPI_FAILURE;

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( hCursor == 0 )
      return DBCAPI_FAILURE;

   if ( AdsGetNumFields( hCursor, &usFieldCount ) != AE_SUCCESS )
      return DBCAPI_FAILURE;
   else
      return usFieldCount;

} /* ads_num_cols */


/*******************************************************************************
* Module       : ads_num_params
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Return the number of parameters in the statement
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_num_params( ADSHANDLE hStatement )
{
   UNSIGNED16 usNumParams;
      
   DBCLogTrace( "ads_num_params" );

   if ( hStatement == 0 )
      return DBCAPI_FAILURE;

   if ( AdsStmtGetNumParams( hStatement, &usNumParams ) != AE_SUCCESS )
      return DBCAPI_FAILURE;
   else
      return usNumParams;

} /* ads_num_params */


/*******************************************************************************
* Module       : ads_num_rows
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Return the number of rows in the cursor
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_num_rows( ADSHANDLE hStatement )
{
   UNSIGNED32 ulRecordCount;
   ADSHANDLE  hCursor;

   DBCLogTrace( "ads_num_rows" );

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( AdsGetRecordCount( hCursor, ADS_RESPECTFILTERS, &ulRecordCount ) != AE_SUCCESS )
      return DBCAPI_FAILURE;
   else
      return ulRecordCount;

} /* ads_num_rows */


/*******************************************************************************
* Module       : ads_prepare
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Prepare a query for execution
* Notes        : 
*******************************************************************************/
ADSHANDLE ENTRYPOINT ads_prepare( a_ads_connection *poConnect, UNSIGNED8 *pucSQL, UNSIGNED8 ucIsUnicode )
{
   ADSHANDLE  hStmt;
   UNSIGNED32 ulRetCode;

   DBCLogTrace( "ads_prepare" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   ulRetCode = AdsCreateSQLStatement( poConnect->hConnect, &hStmt );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( ucIsUnicode )
      ulRetCode = AdsPrepareSQLW( hStmt, (WCHAR*)pucSQL );
   else
      ulRetCode = AdsPrepareSQL( hStmt, pucSQL );
   if ( ulRetCode != AE_SUCCESS )
      {
      AdsSetProperty( poConnect->hConnect, ADS_PUSH_ERROR_STACK, 0 );
      AdsCloseSQLStatement( hStmt );
      AdsSetProperty( poConnect->hConnect, ADS_POP_ERROR_STACK, 0 );
      return DBCAPI_FAILURE;
      }

   return hStmt;

} /* ads_prepare */


/*******************************************************************************
* Module       : ads_reset
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Resets a statement to its prepared state condition.
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_reset( ADSHANDLE hStatement )
{
   ADSHANDLE hCursor;

   DBCLogTrace( "ads_reset" );

   // Preparing the same query twice is a no-op in ACE.
   // All we need to do here is close any open cursor on the STMT.
   if ( hStatement == 0 )
      return DBCAPI_FAILURE;

   if ( AdsStmtGetCursorHandle( hStatement, &hCursor ) != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( hCursor )
      AdsCloseTable( hCursor );

   return DBCAPI_SUCCESS;

} /* ads_reset */


/*******************************************************************************
* Module       : ads_rollback
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Rollback an active transaction
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_rollback( a_ads_connection *poConnect )
{
   DBCLogTrace( "ads_rollback" );

   if ( poConnect == NULL )
      return DBCAPI_FAILURE;

   if ( AdsRollbackTransaction( poConnect->hConnect ) == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_rollback */


/*******************************************************************************
* Module       : ads_send_param_data
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Sends data as part of a bound parameter.
* Notes        : This method can be used to send a large amount of data for a bound parameter in chunks.
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_send_param_data( ADSHANDLE hStatement, UNSIGNED32 ulIndex, UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength )
{
   DBCLogTrace( "ads_send_param_data" );

   if ( hStatement == 0 )
      return DBCAPI_FAILURE;

   if ( AdsSetBinary( hStatement, ADSFIELD( ulIndex + 1 ), ADS_BINARY, ulLength, 0, pucBuffer, ulLength ) == AE_SUCCESS )
      return DBCAPI_SUCCESS;
   else
      return DBCAPI_FAILURE;

} /* ads_send_param_data */


/*******************************************************************************
* Module       : ads_sqlstate
* Created      : 11-04-2010 Peter F
* Last Mod     :
* Return Value : 
* Desc         : Return the 'sql state' of the last error
* Notes        : 
*******************************************************************************/
UNSIGNED32 ENTRYPOINT ads_sqlstate( a_ads_connection *poConnect, UNSIGNED8 *pucBuffer, UNSIGNED32 ulLength )
{
   UNSIGNED32 ulRetCode;
   UNSIGNED32 ulError;
   UNSIGNED8  aucError[ADS_MAX_ERROR_LEN+1];
   UNSIGNED16 usLen;

   DBCLogTrace( "ads_sqlstate" );

   usLen = sizeof( aucError );
   ulRetCode = AdsGetLastError( &ulError, aucError, &usLen );
   if ( ulRetCode != AE_SUCCESS )
      return DBCAPI_FAILURE;

   if ( ulError == AE_SUCCESS )
      strncpy( (SIGNED8*)pucBuffer, "00000", ulLength );
   else
      {
      UNSIGNED8 *pcState;
      #define StateMarker  "State = "

      pcState = (UNSIGNED8*)strstr( (SIGNED8*)aucError, StateMarker );
      if ( pcState == NULL )
         strncpy( (SIGNED8*)pucBuffer, "00000", ulLength );
      else
         {
         pcState += strlen( StateMarker );
         
         /* The state code is a 5 digit alphanumeric. NULL termintate it */
         *(pcState + 5) = 0;
         strncpy( (SIGNED8*)pucBuffer, (SIGNED8*)pcState, ulLength );
         }
      }

   return DBCAPI_SUCCESS;

} /* ads_sqlstate */



#endif  // ACE
