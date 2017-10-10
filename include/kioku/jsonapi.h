/**
 * This is a C API that implements the JSON API Spec.
 * This format will be followed by the Kioku REST API.
 * The API format is described here: http://jsonapi.org/
 *
 * This C API abstracts the underlying JSON implementation by providing a suite of
 * structs and methods to build/inspect JSON API objects in a strongly typed manner.
 *
 * In general, values passed to these methods are copied to simplify memory management.
 */

#define srsJSON_KEY_ERROR "\"error\""
#define srsJSON_SCHEMA_ERROR  "{"srsJSON_KEY_ERROR": {}}"

typedef struct srsJSON_API_s srsJSON_API;
typedef struct srsJSON_ERROR_s srsJSON_ERROR;
typedef struct srsJSON_LINKS_s srsJSON_LINKS;
typedef struct srsJSON_LINK_s srsJSON_LINK;
typedef struct srsJSON_PAGINATION_s srsJSON_PAGINATION;
typedef struct srsJSON_INCLUDED_s srsJSON_INCLUDED;
typedef struct srsJSON_RESOURCE_s srsJSON_RESOURCE;
typedef struct srsJSON_ATTRIBUTES_s srsJSON_ATTRIBUTES;
typedef struct srsJSON_RELATIONSHIPS_s srsJSON_RELATIONSHIPS;
typedef struct srsJSON_REQUEST_s srsJSON_REQUEST;
typedef struct srsJSON_RESPONSE_s srsJSON_RESPONSE;

srsAPI srsBOOL srsJSON_IsValidMember(const srsSTRING member);
srsAPI srsBOOL srsJSON_IsValidValue(const srsSTRING value);
srsAPI srsBOOL srsJSON_IsValidURL(const srsSTRING value);
srsAPI srsBOOL srsJSON_IsValidStatusCode(const srsSTRING value);

srsAPI const srsJSON_REQUEST *srsJSON_Request_Parse(const srsSTRING data);
srsAPI srsBOOL srsJSON_Request_IsValidPointer(const srsJSON_REQUEST *request, const srsSTRING pointer);
srsAPI srsBOOL srsJSON_Request_IsValidParameter(const srsJSON_REQUEST *request, const srsSTRING pointer);
srsAPI srsBOOL srsJSON_Request_GetParameter(const srsJSON_REQUEST *request, const srsSTRING name, srsSTRING param_out, size_t nbytes);

srsAPI srsSIZE srsJSON_Response_Encode(const srsJSON_RESPONSE *response, srsSTRING data_out, srsSIZE nbytes);

srsAPI srsBOOL srsJSON_Response_SetMetaData(srsJSON_RESPONSE *response, const srsSTRING member, const srsSTRING value);

srsAPI srsBOOL srsJSON_Response_SetAPI(srsJSON_RESPONSE *response, const srsJSON_API *api);
srsAPI srsBOOL srsJSON_API_SetVersion(srsJSON_API *response, const srsSTRING version);
srsAPI srsBOOL srsJSON_API_SetMetaData(srsJSON_API *response, const srsSTRING member, const srsSTRING value);

srsAPI srsBOOL srsJSON_Response_SetErrors(srsJSON_RESPONSE *response, const srsJSON_ERROR *errors, srsU16 error_count);
srsAPI srsBOOL srsJSON_Error_SetID(srsJSON_ERROR *error, const srsSTRING id);
srsAPI srsBOOL srsJSON_Error_SetAbout(srsJSON_ERROR *error, const srsJSON_LINK *link);
srsAPI srsBOOL srsJSON_Error_SetStatus(srsJSON_ERROR *error, const srsSTRING status);
srsAPI srsBOOL srsJSON_Error_SetCode(srsJSON_ERROR *error, const srsSTRING code);
srsAPI srsBOOL srsJSON_Error_SetTitle(srsJSON_ERROR *error, const srsSTRING title);
srsAPI srsBOOL srsJSON_Error_SetDetail(srsJSON_ERROR *error, const srsSTRING detail);
srsAPI srsBOOL srsJSON_Error_SetSourcePointer(srsJSON_ERROR *error, const srsSTRING pointer);
srsAPI srsBOOL srsJSON_Error_SetSourceParameter(srsJSON_ERROR *error, const srsSTRING parameter);
srsAPI srsBOOL srsJSON_Error_SetMetaData(srsJSON_ERROR *error, const srsSTRING member, const srsSTRING value);

srsAPI srsBOOL srsJSON_Response_SetIncluded(srsJSON_RESPONSE *response, const srsJSON_RESOURCE *included, srsU16 include_count);
srsAPI srsBOOL srsJSON_Resource_SetID(srsJSON_RESOURCE *resource, const srsSTRING id);
srsAPI srsBOOL srsJSON_Resource_SetType(srsJSON_RESOURCE *resource, const srsSTRING type);
srsAPI srsBOOL srsJSON_Resource_SetAttribute(srsJSON_RESOURCE *resource, const srsSTRING member, const srsSTRING value);
srsAPI srsBOOL srsJSON_Resource_SetRelationships(srsJSON_RESOURCE *resource, const srsJSON_RELATIONSHIPS *relationships);
srsAPI srsBOOL srsJSON_Resource_SetLinks(srsJSON_RESOURCE *resource, const srsJSON_LINKS *links);
srsAPI srsBOOL srsJSON_Resource_SetMetaData(srsJSON_RESOURCE *resource, const srsSTRING member, const srsSTRING value);

srsAPI srsBOOL srsJSON_Response_SetLinks(srsJSON_RESPONSE *response, const srsJSON_LINKS *links);
srsAPI srsBOOL srsJSON_Links_SetSelf(srsJSON_LINKS *links, const srsJSON_LINK *self);
srsAPI srsBOOL srsJSON_Links_SetRelated(srsJSON_LINKS *links, const srsJSON_LINK *related);
srsAPI srsBOOL srsJSON_Links_SetPagination(srsJSON_LINKS *links, const srsJSON_PAGINATION *pagination);

srsAPI void srsJSON_FreeResponse(srsJSON_RESPONSE *response);
srsAPI void srsJSON_FreeRequest(srsJSON_REQUEST *request);
