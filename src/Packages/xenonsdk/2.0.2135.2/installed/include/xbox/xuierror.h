//
//  Values are 32 bit values layed out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_XUI                     0x30


//
// Define the severity codes
//
#define XUI_SEVERITY_SUCCESS             0x0
#define XUI_SEVERITY_ERROR               0x2


//
// MessageId: XUI_ERR_PROP_INVALIDTYPE
//
// MessageText:
//
//  Invalid property type.
//
#define XUI_ERR_PROP_INVALIDTYPE         ((HRESULT)0x80300001L)

//
// MessageId: XUI_ERR_PROP_INVALIDSTATE
//
// MessageText:
//
//  Invalid property state.
//
#define XUI_ERR_PROP_INVALIDSTATE        ((HRESULT)0x80300002L)

//
// MessageId: XUI_ERR_PROP_INVALIDVALUE
//
// MessageText:
//
//  Invalid property value.
//
#define XUI_ERR_PROP_INVALIDVALUE        ((HRESULT)0x80300003L)

//
// MessageId: XUI_ERR_CLASS_NOTREG
//
// MessageText:
//
//  The class was not registered.
//
#define XUI_ERR_CLASS_NOTREG             ((HRESULT)0x80300004L)

//
// MessageId: XUI_ERR_CLASS_ALREADYREG
//
// MessageText:
//
//  The class was already registered.
//
#define XUI_ERR_CLASS_ALREADYREG         ((HRESULT)0x80300005L)

//
// MessageId: XUI_ERR_CLASS_BASENOTREG
//
// MessageText:
//
//  The base class was not registered.
//
#define XUI_ERR_CLASS_BASENOTREG         ((HRESULT)0x80300006L)

//
// MessageId: XUI_ERR_PROP_NOTFOUND
//
// MessageText:
//
//  The property was not found.
//
#define XUI_ERR_PROP_NOTFOUND            ((HRESULT)0x80300007L)

//
// MessageId: XUI_ERR_PROP_TYPEMISMATCH
//
// MessageText:
//
//  Mismatched property types.
//
#define XUI_ERR_PROP_TYPEMISMATCH        ((HRESULT)0x80300008L)

//
// MessageId: XUI_ERR_PROP_INVALIDID
//
// MessageText:
//
//  Invalid property id.
//
#define XUI_ERR_PROP_INVALIDID           ((HRESULT)0x80300009L)

//
// MessageId: XUI_ERR_ELEM_INVALIDOBJ
//
// MessageText:
//
//  Invalid object.
//
#define XUI_ERR_ELEM_INVALIDOBJ          ((HRESULT)0x8030000AL)

//
// MessageId: XUI_ERR_ELEM_NOPARENT
//
// MessageText:
//
//  Element does not have a parent.
//
#define XUI_ERR_ELEM_NOPARENT            ((HRESULT)0x8030000BL)

//
// MessageId: XUI_ERR_PROP_INVALIDINDEX
//
// MessageText:
//
//  Invalid property index.
//
#define XUI_ERR_PROP_INVALIDINDEX        ((HRESULT)0x8030000CL)

//
// MessageId: XUI_ERR_FILE_INVALID
//
// MessageText:
//
//  Invalid (or corrupt) XUI binary file format.
//
#define XUI_ERR_FILE_INVALID             ((HRESULT)0x8030000DL)

//
// MessageId: XUI_ERR_BINARY_STREAM_INVALID_OBJECT
//
// MessageText:
//
//  Invalid object identifier in binary object stream.
//
#define XUI_ERR_BINARY_STREAM_INVALID_OBJECT ((HRESULT)0x8030000EL)

//
// MessageId: XUI_ERR_BINARY_STREAM_INVALID_PROP_DATA
//
// MessageText:
//
//  Invalid (or corrupt) XUI binary file format (bad property data).
//
#define XUI_ERR_BINARY_STREAM_INVALID_PROP_DATA ((HRESULT)0x8030000FL)

//
// MessageId: XUI_ERR_BINARY_STREAM_INVALID_CUSTOM_PROP_DATA
//
// MessageText:
//
//  Custom property FromBinary handler acted unreasonably.
//
#define XUI_ERR_BINARY_STREAM_INVALID_CUSTOM_PROP_DATA ((HRESULT)0x80300010L)

//
// MessageId: XUI_ERR_MESSAGE_NOT_HANDLED
//
// MessageText:
//
//  The message was not handled.
//
#define XUI_ERR_MESSAGE_NOT_HANDLED      ((HRESULT)0x80300011L)

//
// MessageId: XUI_ERR_INVALID_RESOURCE_PATH
//
// MessageText:
//
//  The specified resource path is invalid.
//
#define XUI_ERR_INVALID_RESOURCE_PATH    ((HRESULT)0x80300012L)

//
// MessageId: XUI_ERR_RESOURCE_COULD_NOT_BE_OPENED
//
// MessageText:
//
//  The specified resource path was not found, or could not be opened.
//
#define XUI_ERR_RESOURCE_COULD_NOT_BE_OPENED ((HRESULT)0x80300013L)

//
// MessageId: XUI_ERR_DOES_NOT_WANT_FOCUS
//
// MessageText:
//
//  Attempted to set focus on control which did not want focus.
//
#define XUI_ERR_DOES_NOT_WANT_FOCUS      ((HRESULT)0x80300014L)

//
// MessageId: XUI_ERR_EXPECTED_OUTERMOST
//
// MessageText:
//
//  Expected most derived class or object.
//
#define XUI_ERR_EXPECTED_OUTERMOST       ((HRESULT)0x80300015L)

//
// MessageId: XUI_ERR_CAST_FAILED
//
// MessageText:
//
//  Dynamic cast of class or object failed.
//
#define XUI_ERR_CAST_FAILED              ((HRESULT)0x80300016L)

//
// MessageId: XUI_ERR_ELEM_NOT_FOUND
//
// MessageText:
//
//  Element not found.
//
#define XUI_ERR_ELEM_NOT_FOUND           ((HRESULT)0x80300017L)

//
// MessageId: XUI_ERR_MESSAGE_HANDLING
//
// MessageText:
//
//  Message was handled incorrectly.
//
#define XUI_ERR_MESSAGE_HANDLING         ((HRESULT)0x80300018L)

//
// MessageId: XUI_ERR_RESOURCE_INVALID_OR_CORRUPT
//
// MessageText:
//
//  The specified resource data was invalid, or corrupt.
//
#define XUI_ERR_RESOURCE_INVALID_OR_CORRUPT ((HRESULT)0x8030001AL)

//
// MessageId: XUI_ERR_RESOURCE_UNKNOWN_TRANSPORT
//
// MessageText:
//
//  The specified resource transport was unrecognized.
//
#define XUI_ERR_RESOURCE_UNKNOWN_TRANSPORT ((HRESULT)0x8030001BL)

//
// MessageId: XUI_ERR_INVALID_RESOURCE_PACKAGE
//
// MessageText:
//
//  The specified resource package was invalid, or corrupt.
//
#define XUI_ERR_INVALID_RESOURCE_PACKAGE ((HRESULT)0x8030001CL)

//
// MessageId: XUI_ERR_INVALID_RESOURCE_SEEK_ATTEMPT
//
// MessageText:
//
//  The specified seek offset in the resource was out of bounds.
//
#define XUI_ERR_INVALID_RESOURCE_SEEK_ATTEMPT ((HRESULT)0x8030001DL)

//
// MessageId: XUI_ERR_UNSUPPORTED_VERSION
//
// MessageText:
//
//  The document version is unsupported.
//
#define XUI_ERR_UNSUPPORTED_VERSION      ((HRESULT)0x8030001EL)

//
// MessageId: XUI_ERR_RESOURCE_LOCATOR_MUST_BE_ABSOLUTE
//
// MessageText:
//
//  A resource locator was expected to be absolute, and was not.
//
#define XUI_ERR_RESOURCE_LOCATOR_MUST_BE_ABSOLUTE ((HRESULT)0x8030001FL)

//
// MessageId: XUI_ERR_TYPEFACE_ALREADY_REGISTERED
//
// MessageText:
//
//  The specified typeface name has already been registered.
//
#define XUI_ERR_TYPEFACE_ALREADY_REGISTERED ((HRESULT)0x80300020L)

//
// MessageId: XUI_ERR_SOURCEDATA_ITEM
//
// MessageText:
//
//  Specified source data item is out of range.
//
#define XUI_ERR_SOURCEDATA_ITEM          ((HRESULT)0x80300021L)

//
// MessageId: XUI_ERR_SOURCEDATA_DATUM
//
// MessageText:
//
//  Specified source data datum is out of range.
//
#define XUI_ERR_SOURCEDATA_DATUM         ((HRESULT)0x80300022L)

//
// MessageId: XUI_ERR_DEVICE_NOT_SET
//
// MessageText:
//
//  The target IDirect3DDevice9 has not been set.
//
#define XUI_ERR_DEVICE_NOT_SET           ((HRESULT)0x80300023L)

//
// MessageId: XUI_ERR_CLASS_OBJECTS_EXIST
//
// MessageText:
//
//  The objects of the specified class still exist.
//
#define XUI_ERR_CLASS_OBJECTS_EXIST      ((HRESULT)0x80300024L)

//
// MessageId: XUI_ERR_NESTED_VISUAL
//
// MessageText:
//
//  Visuals cannot be nested.
//
#define XUI_ERR_NESTED_VISUAL            ((HRESULT)0x80300025L)

//
// MessageId: XUI_ERR_VISUAL_NOT_FOUND
//
// MessageText:
//
//  Visual not found.
//
#define XUI_ERR_VISUAL_NOT_FOUND         ((HRESULT)0x80300026L)

//
// MessageId: XUI_ERR_UNEXPECTED_TAG
//
// MessageText:
//
//  Unexpected tag parsing XML extension file.
//
#define XUI_ERR_UNEXPECTED_TAG           ((HRESULT)0x80300027L)

//
// MessageId: XUI_ERR_INVALID_VISUAL
//
// MessageText:
//
//  Invalid or unexpected visual design.
//
#define XUI_ERR_INVALID_VISUAL           ((HRESULT)0x80300028L)

//
// MessageId: XUI_ERR_ELEM_INVALID_HEIGHT
//
// MessageText:
//
//  Object tree height exceeds the limit.
//
#define XUI_ERR_ELEM_INVALID_HEIGHT      ((HRESULT)0x80300029L)

//
// MessageId: XUI_ERR_INTERNAL
//
// MessageText:
//
//  Unexpected internal error.
//
#define XUI_ERR_INTERNAL                 ((HRESULT)0x8030002AL)

//
// MessageId: XUI_ERR_INVALID_CLASS_NAME
//
// MessageText:
//
//  The class name was invalid.
//
#define XUI_ERR_INVALID_CLASS_NAME       ((HRESULT)0x8030002BL)

//
// MessageId: XUI_ERR_INVALID_PNG_FORMAT
//
// MessageText:
//
//  Unsupported PNG format. Only 24bpp and 32bpp PNG files are supported.
//
#define XUI_ERR_INVALID_PNG_FORMAT       ((HRESULT)0x8030002CL)

//
// MessageId: XUI_ERR_INVALID_XML_EXT_VERSION
//
// MessageText:
//
//  Invalid XML extension version  Check the version attribute on the XUIClassExtension tag.
//
#define XUI_ERR_INVALID_XML_EXT_VERSION  ((HRESULT)0x8030002DL)

//
// MessageId: XUI_ERR_INVALID_STRING_TABLE_ENTRY_KEY
//
// MessageText:
//
//  Invalid string table entry key.
//
#define XUI_ERR_INVALID_STRING_TABLE_ENTRY_KEY ((HRESULT)0x8030002EL)

