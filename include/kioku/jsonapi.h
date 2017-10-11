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

typedef struct srsJSON_API_s           srsJSON_API;
typedef struct srsJSON_ERROR_s         srsJSON_ERROR;
typedef struct srsJSON_LINKS_s         srsJSON_LINKS;
typedef struct srsJSON_LINK_s          srsJSON_LINK;
typedef struct srsJSON_PAGINATION_s    srsJSON_PAGINATION;
typedef struct srsJSON_INCLUDED_s      srsJSON_INCLUDED;
typedef struct srsJSON_RESOURCE_s      srsJSON_RESOURCE;
typedef struct srsJSON_ATTRIBUTES_s    srsJSON_ATTRIBUTES;
typedef struct srsJSON_RELATIONSHIPS_s srsJSON_RELATIONSHIPS;
typedef struct srsJSON_REQUEST_s       srsJSON_REQUEST;
typedef struct srsJSON_RESPONSE_s      srsJSON_RESPONSE;

kiokuAPI bool srsJSON_IsValidMember(const char *member);
kiokuAPI bool srsJSON_IsValidValue(const char *value);
kiokuAPI bool srsJSON_IsValidURL(const char *value);
kiokuAPI bool srsJSON_IsValidStatusCode(const char *value);

kiokuAPI const srsJSON_REQUEST *srsJSON_Request_Parse(const char *data);
kiokuAPI bool srsJSON_Request_IsValidPointer(const srsJSON_REQUEST *request, const char *pointer);
kiokuAPI bool srsJSON_Request_IsValidParameter(const srsJSON_REQUEST *request, const char *pointer);
kiokuAPI bool srsJSON_Request_GetParameter(const srsJSON_REQUEST *request, const char *name, char *param_out, size_t nbytes);

kiokuAPI srsSIZE srsJSON_Response_Encode(const srsJSON_RESPONSE *response, char *data_out, srsSIZE nbytes);

kiokuAPI bool srsJSON_Response_SetMetaData(srsJSON_RESPONSE *response, const char *member, const char *value);

kiokuAPI srsJSON_API *srsJSON_Response_AddAPI(srsJSON_RESPONSE *response);
kiokuAPI bool srsJSON_API_SetVersion(srsJSON_API *response, const char *version);
kiokuAPI bool srsJSON_API_SetMetaData(srsJSON_API *response, const char *member, const char *value);

kiokuAPI srsJSON_ERROR *srsJSON_Response_AddError(srsJSON_RESPONSE *response);
kiokuAPI bool srsJSON_Error_SetID(srsJSON_ERROR *error, const char *id);
kiokuAPI bool srsJSON_Error_SetAbout(srsJSON_ERROR *error, const srsJSON_LINK *link);
kiokuAPI bool srsJSON_Error_SetStatus(srsJSON_ERROR *error, const char *status);
kiokuAPI bool srsJSON_Error_SetCode(srsJSON_ERROR *error, const char *code);
kiokuAPI bool srsJSON_Error_SetTitle(srsJSON_ERROR *error, const char *title);
kiokuAPI bool srsJSON_Error_SetDetail(srsJSON_ERROR *error, const char *detail);
kiokuAPI bool srsJSON_Error_SetSourcePointer(srsJSON_ERROR *error, const char *pointer);
kiokuAPI bool srsJSON_Error_SetSourceParameter(srsJSON_ERROR *error, const char *parameter);
kiokuAPI bool srsJSON_Error_SetMetaData(srsJSON_ERROR *error, const char *member, const char *value);

kiokuAPI srsJSON_RESOURCE *srsJSON_Response_AddInclude(srsJSON_RESPONSE *response);
kiokuAPI bool srsJSON_Resource_SetID(srsJSON_RESOURCE *resource, const char *id);
kiokuAPI bool srsJSON_Resource_SetType(srsJSON_RESOURCE *resource, const char *type);
kiokuAPI bool srsJSON_Resource_SetAttribute(srsJSON_RESOURCE *resource, const char *member, const char *value);
kiokuAPI bool srsJSON_Resource_SetRelationships(srsJSON_RESOURCE *resource, const srsJSON_RELATIONSHIPS *relationships);
kiokuAPI bool srsJSON_Resource_SetLinks(srsJSON_RESOURCE *resource, const srsJSON_LINKS *links);
kiokuAPI bool srsJSON_Resource_SetMetaData(srsJSON_RESOURCE *resource, const char *member, const char *value);

kiokuAPI srsJSON_LINKS *srsJSON_Response_AddLinks(srsJSON_RESPONSE *response);
kiokuAPI bool srsJSON_Links_SetSelf(srsJSON_LINKS *links, const srsJSON_LINK *self);
kiokuAPI bool srsJSON_Links_SetRelated(srsJSON_LINKS *links, const srsJSON_LINK *related);
kiokuAPI bool srsJSON_Links_SetPagination(srsJSON_LINKS *links, const srsJSON_PAGINATION *pagination);

kiokuAPI void srsJSON_FreeResponse(srsJSON_RESPONSE *response);
kiokuAPI void srsJSON_FreeRequest(srsJSON_REQUEST *request);
