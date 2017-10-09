/**
 * This is a C API that implements the JSON API Spec.
 * This format will be followed by the Kioku REST API.
 * The API format is described here: http://jsonapi.org/
 *
 * This C API abstracts the underlying JSON implementation by providing a suite of
 * structs and methods to build/inspect JSON API objects in a strongly typed manner.
 */


#define srsJSON_KEY_ERROR "\"error\""
#define srsJSON_SCHEMA_ERROR  "{"srsJSON_KEY_ERROR": {}}"

typedef struct srsJSON_API_s srsJSON_API;

typedef struct srsJSON_ERROR_s srsJSON_ERROR;

typedef struct srsJSON_META_s srsJSON_META;

typedef struct srsJSON_LINKS_s srsJSON_LINKS;

typedef struct srsJSON_LINK_s srsJSON_LINK;

typedef struct srsJSON_INCLUDED_s srsJSON_INCLUDED;

typedef struct srsJSON_RESOURCE_s srsJSON_RESOURCE;

typedef struct srsJSON_RELATIONSHIP_s srsJSON_RELATIONSHIP;

typedef struct srsJSON_REQUEST_s srsJSON_REQUEST;

typedef struct srsJSON_RESPONSE_s srsJSON_RESPONSE;

srsAPI bool srsJSON_IsValidMember(const char *member);

srsAPI bool srsJSON_IsValidValue(const char *value);

srsAPI srsJSON_REQUEST *srsJSON_Request_Parse(const char *data);

srsAPI size_t srsJSON_Response_Encode(const srsJSON_RESPONSE *response, char *data_out, size_t nbytes);

srsAPI bool srsJSON_Response_SetAPI(srsJSON_RESPONSE *response, const char *version);

srsAPI bool srsJSON_Response_SetErrors(srsJSON_RESPONSE *response, const srsJSON_ERROR *errors, uint16_t error_count);

srsAPI bool srsJSON_Response_SetLinks(srsJSON_RESPONSE *response, const srsJSON_LINKS *links);

srsAPI bool srsJSON_Response_SetIncluded(srsJSON_RESPONSE *response, const srsJSON_RESOURCE *included, uint16_t include_count);

srsAPI bool srsJSON_Response_SetMetaData(srsJSON_RESPONSE *response, const char *member, const char *value);
